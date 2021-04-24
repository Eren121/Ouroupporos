from jinja2 import Environment
import pathlib
import glob
import os


def mkdir_p(directory_name):
    pathlib.Path(directory_name).mkdir(parents=True, exist_ok=True)


def set_file_content(filename, content):
    with open(filename, 'w') as file:
        file.write(content)


class CodeGenerator:
    def __init__(self, include_dir_name: str, macro_prefix_name: str, recursion_limit: int = 3):
        # No new line in {% ... %} blocks and trim spaces spaces in front
        self.env = Environment(trim_blocks=True, lstrip_blocks=True)
        self.recursion_limit = recursion_limit
        self.output_directory = "./include/" + include_dir_name
        self.templates_directory = "./template"
        self.macro_prefix_name = macro_prefix_name
        self.header_extension = ".hpp"

        # Create the output directory if it does not exists
        mkdir_p(self.output_directory)

    # Clear all headers file in output directory
    def clear_output_dir(self) -> None:
        files = glob.glob(self.output_directory + "/*" + self.header_extension)
        for f in files:
            os.remove(f)

    # Add a prefix to all C++ macros
    def format_macro(self, name: str) -> str:
        return self.macro_prefix_name + name

    # For C++ inclusion, get the C++ filename from a header
    def format_header(self, name: str) -> str:
        return name + self.header_extension

    def parse_template(self, template_file_name: str) -> str:
        with open(template_file_name, 'r') as file:
            content = file.read()
            template = self.env.from_string(content)
            generated = template.render(recursion_limit=self.recursion_limit,
                                        macro=self.format_macro,
                                        header=self.format_header)
            return generated

    def generate_headers(self) -> None:

        self.clear_output_dir()

        for path_name in glob.glob(self.templates_directory + "/*.jinja2"):
            path = pathlib.Path(path_name)

            source_code = self.parse_template(path_name)

            name = path.stem

            disclaimer_comment = "/// AUTOMATICALLY GENERATED\n/// DO NOT MODIFY\n\n"
            source = disclaimer_comment + source_code
            set_file_content(self.output_directory + "/" + self.format_header(name), source)
