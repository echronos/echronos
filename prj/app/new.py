import imp
import sys
import os

if __name__ == "__main__":
    for pth in ['pystache', 'ply', 'lib']:
        sys.path.insert(0, os.path.join(os.path.dirname(__file__), pth))

    from prj import canonical_paths, xml_parse_file, xml2schema, xml2dict, single_named_child, pystache_render

    # .configuration + .schema (+Python code) -> configuration_object
    # .template + configuration_object -> implementation

    # .schema and .template files are co-located; they are pure source files
    # .configuration is a bit trickier.
    # - applicable to set of .template files (e.g. rtos-*.{c,h})
    # - may need to pass more than one configuration to a template (e.g., rtos configuration, although this feature could be dropped easily enough)
    # - may be generated so are not purely source files, so discovery mechanism must allow them to live in separate directory hierarchy
    # - may be post-processed by Python code to generate configuration object
    # - all file discovery should probably be taken care of by build system
    # - prj should only render templates
    # - the schema and configuration files might be a bit overkill for producing a relatively simple Python data structure with configuration information
    # - the Python configuration data structure could be converted to named tuples to appear a little less free-form
    #   a hierarchy of named tuples could not quite replace the xml schemas because they specify the pre-Python schemas, not the final pystache-level schemas
    #   It would be nice to have a strongly-typed configuration language for this; it would preserve the strict type checking, but otherwise simplify this a fair bit


    template_path, schema_path, configuration_path, configurator_path, output_path = canonical_paths(sys.argv[1:])
    assert os.path.isfile(template_path)
    assert os.path.isfile(schema_path)
    assert os.path.isfile(configuration_path)
    assert os.path.isfile(configurator_path)

    xml_schema_document = xml_parse_file(schema_path)
    schema = xml2schema(xml_schema_document)

    configuration_dom = xml_parse_file(configuration_path)

    configuration = xml2dict(configuration_dom, schema)

    configurator = imp.load_source("__prj.%s" % os.path.splitext(os.path.basename(configurator_path))[0], configurator_path)
    configuration = configurator.configure(configuration)
    cfg = configuration

    import pdb; pdb.set_trace()

    pystache_render(template_path, output_path, configuration)
