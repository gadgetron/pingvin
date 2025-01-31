#!/usr/bin/env python3

from __future__ import annotations

import concurrent.futures
import glob
import hashlib
import os
import pytest
import re
import shutil
import socket
import subprocess
import urllib.error
import urllib.request
import yaml

from pathlib import Path
from dataclasses import dataclass, field
from typing import Dict, List, Callable, Set, Any


def pytest_exception_interact(node, call, report):
    if report.failed and node.config.getoption('--echo-log-on-failure'):
        if hasattr(node, 'funcargs') and 'tmp_path' in node.funcargs:
            tmp_path = node.funcargs['tmp_path']
            for log in glob.glob(os.path.join(tmp_path, '*.log*')):
                with open(log, 'r') as logfile:
                    logdata = logfile.read()
                    report.sections.append((log, logdata))

def pytest_runtest_teardown(item, nextitem):
    if item.config.getoption('--save-results'):
        output_path = item.config.getoption('--save-results')
        output_path = os.path.join(os.path.abspath(output_path), item.callspec.id)
        if hasattr(item, 'funcargs') and 'tmp_path' in item.funcargs:
            tmp_path = item.funcargs['tmp_path']
            shutil.copytree(tmp_path, output_path, dirs_exist_ok=True)


def pytest_addoption(parser):
    parser.addoption(
        '--data-host', action='store', default='https://gadgetronmrd2testdata.blob.core.windows.net/gadgetronmrd2testdata/',
        help='Host from which to download test data.'
    )
    parser.addoption(
        '--cache-disable', action='store_true', default=False,
        help='Disables local caching of data files.'
    )
    parser.addoption(
        '--cache-path', action='store', default=os.path.join(os.path.dirname(__file__), "data"),
        help='Location for storing cached data files.'
    )
    parser.addoption(
        '--ignore-requirements', action='store', nargs='+',
        help="Run tests regardless of whether Pingvin has the specified capabilities."
    )
    parser.addoption(
        '--tags', action='store', nargs='+',
        help='Only run tests with the specified tags.'
    )
    parser.addoption(
        '--echo-log-on-failure', action='store_true', default=False,
        help='Capture and print Pingvin logs on test failure.'
    )
    parser.addoption(
        '--save-results', action='store', default="",
        help='Save Pingvin output and logs in the specified directory.'
    )
    parser.addoption(
        '--download-all', action='store_true', default=False,
        help='Download all test data files before running any tests.'
    )


@pytest.fixture(scope="session")
def data_host_url(request) -> str:
    return request.config.getoption('--data-host')

@pytest.fixture(scope="session")
def cache_disable(request) -> bool:
    return request.config.getoption('--cache-disable')

@pytest.fixture(scope="session")
def cache_path(request, cache_disable) -> Path:
    if cache_disable:
        return None
    return Path(os.path.abspath(request.config.getoption('--cache-path')))

@pytest.fixture(scope="session")
def ignore_requirements(request) -> Set[str]:
    reqs = request.config.getoption('--ignore-requirements')
    if not reqs:
        return set()
    return set(reqs)

@pytest.fixture(scope="session")
def run_tags(request) -> Set[str]:
    tags = request.config.getoption('--tags')
    if not tags:
        return set()
    return set(tags)

@pytest.fixture(scope="session")
def pingvin_capabilities() -> Dict[str,str]:
    command = ["pingvin", "--info"]
    res = subprocess.run(command, capture_output=True, text=True)
    if res.returncode != 0:
        pytest.fail(f"Failed to query Pingvin capabilities... {res.args} return {res.returncode}")

    pingvin_info = res.stderr

    value_pattern = r"(?:\s*):(?:\s+)(?P<value>.*)?"

    capability_markers = {
        'version': "Version",
        'build': "Git SHA1",
        'memory': "System Memory size",
        'cuda_support': "CUDA Support",
        'cuda_devices': "CUDA Device count"
    }

    plural_capability_markers = {
        'cuda_memory': "CUDA Device Memory size",
    }

    def find_value(marker):
        pattern = re.compile(marker + value_pattern, re.IGNORECASE)
        match = pattern.search(pingvin_info)
        if match:
            return match['value']
        else:
            return None

    def find_plural_values(marker):
        pattern = re.compile(marker + value_pattern, re.IGNORECASE)
        return [match['value'] for match in pattern.finditer(pingvin_info)]

    capabilities = {key: find_value(marker) for key, marker in capability_markers.items()}
    capabilities.update({key: find_plural_values(marker) for key, marker in plural_capability_markers.items()})

    print(f"Pingvin capabilities: {capabilities}")
    return capabilities


def pytest_generate_tests(metafunc: pytest.Metafunc) -> None:
    """Dynamically generates a test for each test case file"""
    all_test_specs = []
    for filename in glob.glob('cases/*.yml'):
        spec = Spec.fromfile(filename)
        all_test_specs.append(spec)
    all_test_specs = sorted(all_test_specs, key=lambda s: s.id())
    metafunc.parametrize('spec', all_test_specs, ids=lambda s: s.id())

    if metafunc.config.getoption("--download-all"):
        if metafunc.config.getoption("--cache-disable"):
            pytest.fail("Cannot download all data files when caching is disabled")
        all_test_data = {}
        for spec in all_test_specs:
            all_test_data.update(spec.test_data_files())
        host_url = metafunc.config.getoption("--data-host")
        local_dir = metafunc.config.getoption("--cache-path")
        Fetcher(host_url, local_dir).fetch(all_test_data)


@pytest.fixture
def check_requirements(spec: Spec, pingvin_capabilities: Dict[str,str], ignore_requirements: Set[str], run_tags: Set[str]):
    """Checks whether each test case should be run based on Pingvin capabilities and test tags"""

    # Check tags first
    if len(run_tags) > 0:
        for tag in run_tags:
            if tag not in spec.tags:
                pytest.skip(f"Test missing tag: {tag}")
    if 'skip' in spec.tags:
        pytest.skip("Test was marked as skipped")

    # Then check requirements
    def rules_from_reqs(section):
        class Rule:
            def __init__(self, capability, validator, message):
                self.capability = capability
                self.validator = validator
                self.message = message

            def is_satisfied(self, capabilities):
                value = capabilities.get(self.capability)
                return self.validator(value)

        def parse_memory(string):
            pattern = re.compile(r"(?P<value>\d+)(?: MB)?")
            match = pattern.search(string)
            return float(match['value'])

        def is_enabled(value):
            return value in ['YES', 'yes', 'True', 'true', '1']

        def has_more_than(target):
            return lambda value: value is not None and parse_memory(str(target)) <= parse_memory(value)

        def is_positive(value):
            return value is not None and int(value) > 0

        def each(validator):
            return lambda values: all([validator(value) for value in values])

        rules = [
            ('system_memory', lambda req: Rule('memory', has_more_than(req), "Not enough system memory.")),
            ('gpu_support', lambda req: Rule('cuda_support', is_enabled, "CUDA support required.")),
            ('gpu_support', lambda req: Rule('cuda_devices', is_positive, "Not enough CUDA devices.")),
            ('gpu_memory', lambda req: Rule('cuda_memory', each(has_more_than(req)), "Not enough graphics memory."))
        ]

        return [(key, rule(section[key])) for key, rule in rules if key in section]

    rules = rules_from_reqs(spec.requirements)
    for _, rule in rules:
        if rule.capability in ignore_requirements:
            continue
        if not rule.is_satisfied(pingvin_capabilities):
            pytest.skip(rule.message)

@pytest.fixture
def local_test_data_path(cache_path: Path, tmp_path: Path):
    # If cache_path is disabled, the fetched data will live in the test working directory (tmp_path)
    # PyTest automatically cleans up these directories after 3 runs
    def _local_test_data_path(filename: str) -> str:
        if not cache_path:
            return os.path.join(tmp_path, filename)
        return os.path.join(cache_path, filename)
    return _local_test_data_path

@pytest.fixture
def fetch_test_data(data_host_url: str, local_test_data_path) -> Callable:
    """Fetch test data for an individual test case"""
    def _fetch_test_data(test_files: Dict[str,str]):
        local_dir = local_test_data_path("")
        Fetcher(data_host_url, local_dir).fetch(test_files)
    return _fetch_test_data


class Fetcher:
    """Concurrently fetches test data from a remote host."""
    def __init__(self, host_url: str, local_dir: Path):
        self.host_url = host_url
        self.local_dir = local_dir

    def fetch(self, test_files: Dict[str,str]) -> List[str]:
        with concurrent.futures.ThreadPoolExecutor() as executor:
            results = list(executor.map(self.download_item, test_files.items()))
        return results

    def download_item(self, item):
        """Fetches test data from the remote data host and caches it locally"""
        filename, checksum = item

        destination = os.path.join(self.local_dir, filename)
        need_to_fetch = True
        if os.path.exists(destination):
            if not os.path.isfile(destination):
                raise RuntimeError(f"Destination '{destination}' exists but is not a file")

            if not self.is_valid(destination, checksum):
                print(f"Destination '{destination}' exists file but checksum does not match... Forcing download")
            else:
                need_to_fetch = False

        if need_to_fetch:
            print(f"Fetching test data: {filename}")
            os.makedirs(os.path.dirname(destination), exist_ok=True)
            url = f"{self.host_url}{filename}"
            self.urlretrieve(url, destination)

        if not self.is_valid(destination, checksum):
            raise RuntimeError(f"Downloaded file '{destination}' does not match checksum")
        return destination


    def is_valid(self, file: Path, digest: str) -> bool:
        if not os.path.isfile(file):
            return False
        def compute_checksum(file: Path) -> str:
            md5 = hashlib.new('md5')
            with open(file, 'rb') as f:
                for chunk in iter(lambda: f.read(65536), b''):
                    md5.update(chunk)
            return md5.hexdigest()
        return digest == compute_checksum(file)

    def urlretrieve(self, url: str, filename: str, retries: int = 5) -> str:
        if retries <= 0:
            raise RuntimeError("Download from {} failed".format(url))
        try:
            with urllib.request.urlopen(url, timeout=60) as connection:
                with open(filename,'wb') as f:
                    for chunk in iter(lambda : connection.read(1024*1024), b''):
                        f.write(chunk)
                return connection.headers["Content-MD5"]
        except (urllib.error.URLError, ConnectionResetError, socket.timeout) as exc:
            print("Retrying connection for file {}, reason: {}".format(filename, str(exc)))
            return self.urlretrieve(url, filename, retries=retries-1)

@dataclass
class Spec():
    """Defines a test case specification"""

    @dataclass
    class Job():
        """Defines a job to be run by Pingvin"""
        name: str
        datafile: str
        checksum: str
        args: List[str]

        @staticmethod
        def fromdict(config: Dict[str, str], name: str) -> Spec.Job:
            if not config:
                return None

            datafile = config['data']
            if not datafile:
                raise ValueError(f"Missing 'data' key in {name} configuration")

            checksum = config['checksum']
            if not checksum:
                raise ValueError(f"Missing 'checksum' key in {name} configuration")

            args = []
            if 'run' in config:
                for run in config['run']:
                    args.append(run['args'])
            else:
                args.append(config['args'])

            return Spec.Job(name=name, datafile=datafile, checksum=checksum, args=args)

    @dataclass
    class ImageSeriesTest():
        """Defines a test for an image series comparison"""
        image_series: int
        scale_comparison_threshold: float
        value_comparison_threshold: float

    @dataclass
    class Validation():
        """Defines a validation test for the output of a job"""
        reference: str
        checksum: str
        image_series_tests: List[ImageSeriesTest]

        @staticmethod
        def fromdict(config: Dict[str, str]) -> Spec.Validation:
            if not config:
                return None
            reference = config['reference']
            if not reference:
                raise ValueError("Missing 'reference' key in validation configuration")
            checksum = config['checksum']
            if not checksum:
                raise ValueError("Missing 'checksum' key in validation configuration")
            tests = config['tests']
            if not tests:
                raise ValueError("Missing 'tests' key in validation configuration")
            if not isinstance(tests, list):
                raise ValueError("Key 'tests' should be a list in validation configuration")

            image_series_tests = []
            for test in tests:
                num = test['image_series']
                st = test.get('scale_comparison_threshold', 0.01)
                vt = test.get('value_comparison_threshold', 0.01)
                image_series_tests.append(
                    Spec.ImageSeriesTest(image_series=num, scale_comparison_threshold=st, value_comparison_threshold=vt)
                )

            return Spec.Validation(reference=reference, checksum=checksum,
                    image_series_tests=image_series_tests)

    name: str
    tags: Set[str] = field(default_factory=set)
    requirements: Dict[str, str] = field(default_factory=dict)

    dependency: Spec.Job = None
    reconstruction: Spec.Job = None
    validation: Spec.Validation = None

    def id(self):
        return f"{self.name}"

    def test_data_files(self):
        files = {}
        if self.dependency is not None:
            files[self.dependency.datafile] = self.dependency.checksum
        files[self.reconstruction.datafile] = self.reconstruction.checksum
        files[self.validation.reference] = self.validation.checksum
        return files

    @staticmethod
    def fromfile(filename: str) -> Spec:
        with open(filename, 'r') as file:
            parsed = yaml.safe_load(file)
            name = os.path.relpath(filename)
            spec = Spec(name=name)

            tags = parsed.get('tags', None)
            if not tags:
                tags = []
            if not isinstance(tags, list):
                tags = [tags]
            spec.tags = set(tags)

            requirements = parsed.get('requirements', None)
            if not requirements:
                requirements = {}
            if not isinstance(requirements, dict):
                raise ValueError(f"Invalid requirements in {filename}")
            spec.requirements = requirements

            spec.dependency = Spec.Job.fromdict(parsed.get('dependency', None), 'dependency')
            spec.reconstruction = Spec.Job.fromdict(parsed['reconstruction'], 'reconstruction')
            spec.validation = Spec.Validation.fromdict(parsed.get('validation', None))

            return spec