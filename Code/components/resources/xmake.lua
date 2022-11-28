local function include_resources()
	add_packages("zlib", "glm")
end

component("Resources")
	include_resources()
unittest("Resources")
	include_resources()