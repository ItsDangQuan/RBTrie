include(FetchContent)

FetchContent_Declare(
	uni-algo
	URL https://github.com/uni-algo/uni-algo/archive/refs/tags/v1.2.0.zip
	URL_HASH SHA1=2476419cf95dbde2b07a510206274908379503bb
)

add_subdirectory("uni-algo")
