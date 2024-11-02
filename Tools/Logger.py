#------------------------------------------------------------------------------
#
#
#------------------------------------------------------------------------------

class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


class Logger:
    def __init__(self, name="Logger"):
        self.name = name

    def _format_message(self, level, message, color):
        #return f"{color}[{self.name} - {level}] {message}{Colors.ENDC}"
        return f"{color}[{self.name}] [{level}] {message}{Colors.ENDC}"

    def info(self, message):
        print(self._format_message("INFO", message, Colors.OKBLUE))

    def success(self, message):
        print(self._format_message("SUCCESS", message, Colors.OKGREEN))

    def warning(self, message):
        print(self._format_message("WARNING", message, Colors.WARNING))

    def error(self, message):
        print(self._format_message("ERROR", message, Colors.FAIL))

    # TODO
    def log(self, message, level):
        print(self._format_message(level, message, Colors.OKBLUE))
