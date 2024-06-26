add_test(NAME travis_ci
	COMMAND test.sh ${CMAKE_BINARY_DIR}/bin
	)

add_custom_target( SZ_data ALL
        getData.sh
	COMMENT "Downloading Data For Testing"
	)

file(COPY
        sz.config
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
	)
