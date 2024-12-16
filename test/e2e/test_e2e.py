from __future__ import annotations

import os
import mrd
import pytest
import itertools
import subprocess
import numpy
import numpy.typing as npt

from typing import Dict, List, Callable, Set, Any


def test_e2e(spec, check_requirements, request, fetch_test_data, process_data, validate_output):
    """The main test function for each test case.

    One instance is created for each case file in ./cases/.
    These instances and related fixtures are defined in conftest.py.
    """
    if not request.config.getoption("--download-all"):
        fetch_test_data(spec.test_data_files())

    if spec.dependency is not None:
        process_data(spec.dependency)
    output_file = process_data(spec.reconstruction)
    validate_output(spec.validation, output_file)


@pytest.fixture
def process_data(local_test_data_path, tmp_path):
    """Runs the Pingvin on the input test data, producing an output file."""
    def _process_data(job):
        input_file = local_test_data_path(job.datafile)
        output_file = os.path.join(tmp_path, job.name + ".output.mrd")

        invocations = []
        for args in job.args:
            invocations.append(f"pingvin {args}")
        invocations[0] += f" --input {input_file}"
        invocations[-1] += f" --output {output_file}"

        command = " | ".join(invocations)
        command = ['bash', '-c', command]

        print(f"Run: {command}")

        log_stdout_filename = os.path.join(tmp_path, f"pingvin_{job.name}.log.out")
        log_stderr_filename = os.path.join(tmp_path, f"pingvin_{job.name}.log.err")
        with open(log_stdout_filename, 'w') as log_stdout:
            with open(log_stderr_filename, 'w') as log_stderr:
                result = subprocess.run(command, stdout=log_stdout, stderr=log_stderr, cwd=tmp_path)
                if result.returncode != 0:
                    pytest.fail(f"Pingvin failed with return code {result.returncode}. See {log_stderr_filename} for details.")

        return output_file

    return _process_data

@pytest.fixture
def validate_output(local_test_data_path):
    """Validates each image (data and header) in the output file against the reference file."""
    def _validate_output(spec: Spec.Validation, output_file: str) -> None:
        reference_file = local_test_data_path(spec.reference)
        reference_images = extract_image_data(reference_file)
        output_images = extract_image_data(output_file)

        for test in spec.image_series_tests:
            ref_data = reference_images[test.image_series]['data']
            out_data = output_images[test.image_series]['data']
            validate_image_data(out_data, ref_data, test.scale_comparison_threshold, test.value_comparison_threshold)

            ref_headers = reference_images[test.image_series]['headers']
            out_headers = output_images[test.image_series]['headers']
            for ref, out in itertools.zip_longest(ref_headers, out_headers):
                validate_image_header(out, ref)

    return _validate_output

def extract_image_data(filename: Path) -> Dict[int, Dict[str, Any]]:
    dataset = {}
    with mrd.BinaryMrdReader(filename) as reader:
        _ = reader.read_header()
        for item in reader.read_data():
            if type(item.value).__name__ != "Image":
                continue
            head = item.value.head
            if head.image_series_index not in dataset:
                dataset[head.image_series_index] = []
            dataset[head.image_series_index].append(item.value)
    res = {}
    for series, images in dataset.items():
        res[series] = {}
        res[series]['data'] = numpy.stack([img.data for img in images])
        res[series]['headers'] = map(lambda img: img.head, images)
        res[series]['meta'] = map(lambda img: img.meta, images)
    return res

def validate_image_data(output_data: npt.ArrayLike, reference_data: npt.ArrayLike,
                scale_threshold: float, value_threshold: float) -> None:
    assert output_data.shape == reference_data.shape
    assert output_data.dtype == reference_data.dtype

    ref_data = reference_data.flatten().astype('float32')
    out_data = output_data.flatten().astype('float32')

    norm_diff = numpy.linalg.norm(out_data - ref_data) / numpy.linalg.norm(ref_data)
    scale = numpy.dot(out_data, out_data) / numpy.dot(out_data, ref_data)

    if value_threshold < norm_diff:
        pytest.fail("Comparing values, norm diff: {} (threshold: {})".format(norm_diff, value_threshold))

    if scale_threshold < abs(1 - scale):
        pytest.fail("Comparing image scales, ratio: {} ({}) (threshold: {})".format(scale, abs(1 - scale),
                                                                                        scale_threshold))

def validate_image_header(output_header: mrd.ImageHeader, reference_header: mrd.ImageHeader) -> None:
    def equals():
        # Account for *converted* reference header field having value 0 instead of None
        return lambda out, ref: out == ref or (out is None and ref == 0)

    def approx(threshold=1e-6):
        return lambda out, ref: abs(out - ref) <= threshold

    def ignore():
        return lambda out, ref: True

    def each(rule):
        return lambda out, ref: all(rule(out, ref) for out, ref in itertools.zip_longest(out, ref))

    header_rules = {
        'flags': equals(),
        'measurement_uid': equals(),
        'field_of_view': each(approx()),
        'position': each(approx()),
        'col_dir': each(approx()),
        'line_dir': each(approx()),
        'slice_dir': each(approx()),
        'patient_table_position': each(approx()),
        'average': equals(),
        'slice': equals(),
        'contrast': equals(),
        'phase': equals(),
        'repetition': equals(),
        'set': equals(),
        'acquisition_time_stamp': ignore(),
        'physiology_time_stamp': each(ignore()),
        'image_type': equals(),
        'image_index': equals(),
        'image_series_index': equals(),
        # Ignore user values since ISMRMRD->MRD converted files always have user_int/user_float==[0,0,0,0,0,0,0,0]
        'user_int': each(ignore()),
        'user_float': each(ignore())
    }

    for attribute, rule in header_rules.items():
        if not rule(getattr(output_header, attribute), getattr(reference_header, attribute)):
            pytest.fail(f"Image header '{attribute}' does not match reference"
                f" (series {output_header.image_series_index}, index {output_header.image_index})"
                f" [{getattr(output_header, attribute)} != {getattr(reference_header, attribute)}]")
