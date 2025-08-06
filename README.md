# WorldToJsonExporter Plugin

## Overview
This plugin provides a Blueprint SceneComponent of Type ```JsonExport```. This allows for Unreal scenes to export Json files

## How to install

1. Copy the root of repository.
2. Paste into desired project into 'Plugins' folder.
3. Launch the .uproject


## How to use

This plugin doesn't have its own content, but needs to be used in other Blueprints.

### Creating exportable Blueprints

In order to use this, create a new Blueprint which inherits from ```JsonExport``` and override the function ```GetJson```.
This function returns the following:
- A ```JsonObjectWrapper```, which values are added within this function.
- An AttributeId/FieldName, which is the FieldName of this JsonObject.
- A boolean, which specifies if the function succeeded correctly or not.

In addition, the default variables of the class might need tweaking:
- IsWrapper: When enabled, child components will be wrapped within the ```JsonObjectWrapper``` provided by this component.
- Allow Empty: Creates a Json field, regardless whether there are other fields within the ```JsonObjectWrapper```
- AutoParent: Will parent the ```JsonObjectWrapper``` automatically to its hierarchal parent within a Blueprint
- Enabled: Whether to export or skip it.

### Creating Jsons

A couple of functions are provided to create Jsons:
- ```CreateJsonForCurrentWorld```: Will use the current Editor Level to create a Json with all the ```JsonExport``` components within that level.
- ```CreateJsonForWorld```: Will use a specified Level to create a Json with all the ```JsonExport``` components within that Level.
- ```CreateJsonForActor```: Will use a specified Actor to create a Json with all the ```JsonExport``` components within that Actor.
- ```CreateJsonForSceneComponent```: Will create a Json for the specified ```JsonExport```

These functions are within a static library and can be accessed anywhere within a Blueprint.

### PreGetJsonSync Interface

Any Actor can inherit the Interface ```PreGetJsonSync```. This hooks the Actor to an Event which is called before ```JsonExport``` components their ```GetJson``` functions are called.
Use in order to sync up any variables to its desired state before the Json export process begins.