from collections import namedtuple
import pystache

Config = namedtuple("Config", ("tasks",))
Task = namedtuple("Task", ("name", "stacksize"))

config = Config((Task("a", 1), Task("b", 2)))

renderer = pystache.renderer.Renderer()
pt = pystache.parser.parse("Hello {{#tasks}}Task {{name}}, you have {{stacksize}} bytes of stack space\n{{/tasks}}")
out = renderer.render(pt, config)
print(out)
