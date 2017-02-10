import json
from collections import namedtuple

############################################################
# COMMON
############################################################
class Name:
    def __init__(self, name, native_name=False):
        self.native_name = native_name
        if native_name:
            self.chunks = [name]
        else:
            self.chunks = name.split(' ')

class Type:
    def __init__(self, name, record, native=False):
        self.record = record
        self.dict_name = name
        self.name = Name(name, native_name=native)
        self.category = record['category']

EnumValue = namedtuple('EnumValue', ['name', 'value'])
class EnumType(Type):
    def __init__(self, name, record):
        Type.__init__(self, name, record)
        self.values = [EnumValue(Name(m['name']), m['value']) for m in self.record['values']]

class NativeType(Type):
    def __init__(self, name, record):
        Type.__init__(self, name, record, native=False)

Method = namedtuple('Method', ['name', 'return_type', 'arguments'])
MethodArgument = namedtuple('MethodArgument', ['name', 'type', 'annotation'])
class ObjectType(Type):
    def __init__(self, name, record):
        Type.__init__(self, name, record)
        self.methods = []

############################################################
# PARSE
############################################################
def link_object(obj, types):
    def make_argument(record):
        return MethodArgument(Name(record['name']), types[record['type']], record['annotation'])

    def make_method(record):
        return Method(Name(record['name']), types[record['returns']], [make_argument(a) for a in record['args']])

    if 'methods' in obj.record:
        obj.methods = [make_method(m) for m in obj.record['methods']]

def parse_json(json):
    category_to_parser = {
        'enum': EnumType,
        'native': NativeType,
        'object': ObjectType,
    }

    types = {}

    by_category = {}
    for name in category_to_parser.keys():
        by_category[name] = []

    for (name, record) in json.items():
        category = record['category']
        parsed = category_to_parser[category](name, record)
        by_category[category].append(parsed)
        types[name] = parsed

    for obj in by_category['object']:
        link_object(obj, types)

#############################################################
# OUTPUT
#############################################################

if __name__ == '__main__':
    with open('next.json') as f:
        api_json = json.loads(f.read())
        api = parse_json(api_json)
