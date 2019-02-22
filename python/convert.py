import xml.etree.ElementTree as ET

filename = r'C:\Users\Dmitry\Desktop\Inkscape.logo.svg'

xml = ET.parse(filename)
root = xml.getroot()

if root.tag != 'svg':
    pass #TODO: validate xml

g = root[0]

for path in g:
    d = path.attrib['d']
    print(d, '\n')