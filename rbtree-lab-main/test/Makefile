# .PHONY: 이 타겟들은 실제 파일이 아니므로 항상 실행됩니다.
.PHONY: help build test clean drive

# help: 사용 가능한 모든 명령어를 예쁘게 출력합니다.
help: ## Shows help for all available commands
# http://marmelab.com/blog/2016/02/29/auto-documented-makefile.html
	@grep -E '^[a-zA-Z0-9_%/-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

# build: 'src' 디렉터리에서 소스 코드를 빌드합니다.
build: ## Build executables
	$(MAKE) -C src

# test: 'test' 디렉터리에서 테스트를 실행합니다.
test: ## Test rbtree implementation
	$(MAKE) -C test test

# drive: 코드를 빌드하고, 실행한 뒤, 자동으로 정리합니다.
# 
# [수정된 부분]
# 실행 파일 경로를 './src/main'에서 './src/driver'로 변경했습니다.
# 'src/driver.c'를 컴파일하면 'driver' 실행 파일이 생성될 것이기 때문입니다.
drive: ## Build, run, and then clean the project
	@echo "--- Starting build... ---"
	$(MAKE) build
	@echo "\n--- Running executable ---"
	./src/driver
	@echo "\n--- Execution finished. Cleaning up... ---"
	$(MAKE) clean
	@echo "--- Cleanup complete. ---"

# clean: 빌드 환경을 정리합니다.
clean: ## Clear build environment
	$(MAKE) -C src clean
	$(MAKE) -C test clean
