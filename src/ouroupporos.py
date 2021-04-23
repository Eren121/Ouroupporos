from jinja2 import Environment
import pathlib
import glob

def mkdir_p(directory_name):
    pathlib.Path(directory_name).mkdir(parents=True, exist_ok=True)

def set_file_content(filename, content):
    with open(filename, 'w') as file:
        file.write(content)

class Ouroupporos:
    def __init__(self, recursion_limit = 3):
        # No new line in {% ... %} blocks and trim spaces spaces in front
        self.env = Environment(trim_blocks=True, lstrip_blocks=True)
        self.recursion_limit = recursion_limit
        self.output_directory = "./include"
        self.templates_directory = "./template"

        # Create the output directory if it does not exists
        mkdir_p(self.output_directory)

    # Add a prefix to all C++ macros
    def format_macro(self, name):
        return "OUROUPPOROS_" + name

    # For C++ inclusion, get the C++ filename from a header
    def format_header(self, name):
        return name + ".hpp"

    def parse_template(self, template_file_name):
        with open(template_file_name, 'r') as file:
            content = file.read()
            template = self.env.from_string(content)
            generated = template.render(recursion_limit=self.recursion_limit,
                                        macro=self.format_macro,
                                        header=self.format_header)
            return generated

    def generate_headers(self):
        for path_name in glob.glob(self.templates_directory + "/*.jinja2"):
            path = pathlib.Path(path_name)

            source_code = self.parse_template(path_name)

            name = path.stem
            set_file_content(self.output_directory + "/" + self.format_header(name), source_code)