import json
import os
import inspect
import mido
from typing import Optional

class Config:
    def __init__(self):

        self.patterns = ['up', 'down', 'updown', 'random']
        self.max_octaves = 3

        self.config_file = 'config.json'

        self.inport_name: str = 'reface CP MIDI 1'
        self.outport_name: str = 'reface CP MIDI 1'

        self.pattern: str = 'up'
        self.octaves: int = 2
        self.bpm: int = 300
        self.hold: bool = False
        self.swing: int = 0
        self.channel: int = 0   # omni

        self.load_config()

    def load_config(self):
        """
        Load configuration from the JSON file. Errors:

            1. No file or malformed file - print message and use default values
            2. Missing value - use default
            3. Bad value name or type - print error message and use default
        """
        if not os.path.exists(self.config_file):
            print(f"Configuration file '{self.config_file}' not found. Using default configuration.")
            return

        try:
            with open(self.config_file, 'r') as file:
                config_data = json.load(file)
        except json.JSONDecodeError as e:
            print(f"Configuration file '{self.config_file}' contains JSON error. Using default configuration.")
            print(f"JSON Error: {e}")
            return

        for key, value in config_data.items():
            if hasattr(self, key):
                attr_type = type(getattr(self, key))
                if not isinstance(value, attr_type):
                    print(f"Expected '{key}' to be {attr_type.__name__}, got {type(value)}. Using default value.")
                    continue
                setattr(self, key, value)

    def save_config(self):
        """
        Save the current configuration to the JSON file.
        """
        config_data = {key: value for key, value in inspect.getmembers(self) if not key.startswith('__') and not callable(value) and key != 'config_file'}

        with open(self.config_file, 'w') as file:
            json.dump(config_data, file, indent=4)

    def update_config(self, **kwargs):
        """
        Update the configuration with new values.

        Args:
            **kwargs: Keyword arguments of configuration values to update.
        """
        for key, value in kwargs.items():
            if hasattr(self, key):
                attr_type = type(getattr(self, key))
                if not isinstance(value, attr_type):
                    raise TypeError(f"Expected '{key}' to be {attr_type.__name__}, got {type(value)}")
                setattr(self, key, value)
        self.save_config()

    def next_pattern(self):
        pattern_index = next((i for i, x in enumerate(self.patterns) if x == self.pattern), None)
        pattern_index = (pattern_index + 1) % len(self.patterns)
        self.pattern = self.patterns[pattern_index]

    def next_octaves(self):
        self.octaves = (self.octaves % self.max_octaves) + 1

    def toggle_hold(self):
        self.hold = not self.hold

    def increment_bpm(self):
        self.bpm += 10

    def decrement_bpm(self):
        self.bpm -= 10

    def get_inputs(self):
        return mido.get_input_names()

    def set_input(self, input_name):
        self.inport_name = input_name

    def get_outputs(self):
        return mido.get_output_names()

    def set_output(self, output_name):
        self.outport_name = output_name


# Example usage
if __name__ == "__main__":
    config = Configuration()

    # Accessing configuration values
    print(config.server_ip)
    print(config.port)

    # Updating configuration values
    config.update_config(server_ip='192.168.1.100', port=8080)

    # Saving configuration to file
    config.save_config()
