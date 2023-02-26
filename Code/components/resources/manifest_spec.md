# Resource manifest spec

The resource manifest is a ini file that describes the resource. It is used for keeping track of metadata about the resource, and for defining the (optional) scripting entry point. All resources haave to provide a manifest to avoid chaos and provide versioning.

## Resource structure

Next to the server, there is the resources folder. The resource folder a folder for each resource and within it, its corresponding manifest.

```
└───resources
    ├──a
    │  └──resource-a.manifest
    └──b
       └──resource-b.manifest
```

## Manifest fields

The order of the manifest entries does not matter. The manifest entries are case sensitive.

- `name`: The name of the resource.
- `version`: Your own version of the resource. Numeric and should follow [semver](https://semver.org/).
- `apiset`: Which version of the scripting api the resources subscribes to, in [semver](https:://semver.org).
- `entrypoint`: The name of the entrypoint script. This is optional, and defaults to nothing. The scripting system will determine the runtime based on the extension specified, so .lua files will be fed to a runtime that indicates it supports Lua, and so on.
- `dependencies`: A list of dependencies. The dependencies are specified as `resource_name@version`. The version is optional, and defaults to `*`. The version is specified in [semver](https://semver.org/). The dependencies are resolved in the order they are specified. If a dependency is not found, the resource will not be loaded. If a dependency is found, but the version is not compatible, the resource will not be loaded.
- `description`: A short description of the resource.
- `keywords`: (*Optional*) A list of keywords that describe the resource.
- `license`: (*Optional*) The license that the resource is released under.
- `repository`: (*Optional*) The URL of the repository that the resource is hosted in.
- `homepage`: (*Optional*) The URL of the resource's homepage.

## Example

```ini
[Resource]
name = "my-resource"
version = 1.0.0
apiset = 1.0.0
description = "A resource that does stuff"
keywords = ["selfmade", "resource"]
license = "MIT"
repository = ""
homepage = ""
entrypoint = "test.lua"
dependencies = ["dependency-a@1.0.0", "dependency-b@1.0.0"]

```
