
set(pingvin_legacy_python_config_files
		legacy/config/pseudoreplica.xml
		legacy/config/python_buckets.xml
		legacy/config/python_short.xml
		legacy/config/python_image_array_recon.xml
		legacy/config/python_passthrough.xml
		legacy/config/python_short.xml)

set(pingvin_legacy_python_gadgets_files
		legacy/gadgets/accumulate_and_recon.py
        legacy/gadgets/bucket_recon.py
        legacy/gadgets/image_array_recon.py
		legacy/gadgets/image_array_recon_rtcine_plotting.py
		legacy/gadgets/passthrough.py
		legacy/gadgets/passthrough_array_image.py
		legacy/gadgets/pseudoreplicagather.py
		legacy/gadgets/remove_2x_oversampling.py
		legacy/gadgets/rms_coil_combine.py)

source_group(config FILES ${pingvin_legacy_python_config_files})
source_group(gadgets FILES ${pingvin_legacy_python_gadgets_files})


set(pingvin_python_config_files
		config/Generic_Cartesian_Grappa_RealTimeCine_Python.xml)

install(FILES ${pingvin_python_config_files} DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH} COMPONENT main)