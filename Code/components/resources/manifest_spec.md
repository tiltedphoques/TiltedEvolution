# Resource manifest spec 
The resource manifest is a ini file that describes the resource. It is used for keeping track of metadata about the resource, and for defining the resource entrypoint(s).

## Manifest fields
- `name`: The name of the resource.
- `version`: The version of the resource. This is a string, and should follow [semver](https://semver.org/).
- `apiset`: Which version of the scripting api we subscribe to.
- `description`: A short description of the resource.
- `keywords`: A list of keywords that describe the resource.
- `license`: The license that the resource is released under.
- `repository`: The URL of the repository that the resource is hosted in.
- `homepage`: The URL of the resource's homepage.
- `entrypoints`: A list of entrypoints that the resource provides. See [entrypoints](#entrypoints) for more information.

## Example
```ini
name = "my-resource"
version = "1.0.0"
apiset = "1.0.0"
description = "A resource that does stuff"
keywords = ["example", "resource"]
license = "MIT"
repository = ""
homepage = ""
entrypoints = ["test.lua", "test2.lua"]
```

## Entrypoints
Entry points are the entry points of the resource. They are defined in the `entrypoints` field of the manifest. They are defined as a list of strings, where each string is a path to a file that exports a function. The function is then called when the resource is started.