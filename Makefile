export PATH := $(shell pwd)/scripts:$(PATH)

BUILD_DIR ?= $(BUILD_DIR_BASE)_$(CONFIG)

CLANG_TIDY_ARGS = ./scripts/run-clang-tidy.py -clang-tidy-binary clang-tidy-12 -clang-apply-replacements-binary clang-apply-replacements-12 -j $(shell nproc)

.PHONY: tidy-check
tidy-check:
	${CLANG_TIDY_ARGS} -quiet -p $(BUILD_DIR) -header-filter $(shell realpath ./lt) $(shell realpath ./)

.PHONY: tidy-fix
tidy-fix:
	${CLANG_TIDY_ARGS} -fix -quiet -p $(BUILD_DIR) -header-filter $(shell realpath ./lt) $(shell realpath ./)


.PHONY: coverage
coverage:
	cmake -S . -G Ninja -B cmake-build-coverage -D CMAKE_BUILD_TYPE=Debug -D LT_BUILD_COVERAGE=TRUE
	cmake --build cmake-build-coverage
	cd cmake-build-coverage && ctest -j 12
	# ./cmake-build-coverage/test/bpm_detector ~/Downloads/Techno_Untitled_56\ Kick\ Main\ \(online-audio-converter.com\).wav

.PHONY: coverage-html
coverage-html: coverage
	cd cmake-build-coverage && gcovr --html --html-details --exclude-unreachable-branches -o coverage.html -r ../ -j ${shell nproc} -s .

.PHONY: coverage-xml
coverage-xml: coverage
	cd cmake-build-coverage && gcovr --xml-pretty --exclude-unreachable-branches -o coverage.xml  -r ../ -j ${shell nproc} -s .

.PHONY: stats
stats:
	@cloc -by-file-by-lang --exclude-dir=3rd_party --exclude-ext=svg --vcs=git .

.PHONY: format
format:
	@find lt -iname '*.hpp' -o -iname '*.h' -o -iname '*.cpp' | xargs clang-format-12 -i
	@find test -iname '*.hpp' -o -iname '*.h' -o -iname '*.cpp' | xargs clang-format-12 -i

.PHONY: format-check
format-check:
	@find lt -iname '*.hpp' -o -iname '*.h' -o -iname '*.cpp' | xargs -n 1 -P 1 -I{} -t sh -c 'clang-format-12 -style=file {} | diff - {}'
	@find test -iname '*.hpp' -o -iname '*.h' -o -iname '*.cpp' | xargs -n 1 -P 1 -I{} -t sh -c 'clang-format-12 -style=file {} | diff - {}'