"""Compare saved Bambu test projects after rejected/aborted editor operations."""
import argparse
import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET
import zipfile

NS = {'c': 'http://schemas.microsoft.com/3dmanufacturing/core/2015/02'}


def inspect(path):
    with zipfile.ZipFile(path) as archive:
        if archive.testzip() is not None:
            raise ValueError('Corrupt project ZIP')
        models = [ET.fromstring(archive.read(name)) for name in sorted(archive.namelist()) if name.endswith('.model')]
        meshes = [
            ([tuple(float(v.attrib[k]) for k in ('x', 'y', 'z')) for v in model.findall('.//c:vertex', NS)],
             [tuple(int(t.attrib[k]) for k in ('v1', 'v2', 'v3')) for t in model.findall('.//c:triangle', NS)])
            for model in models]
        transforms = [
            [(node.tag, {k: v for k, v in node.attrib.items() if not k.endswith('UUID')})
             for node in model.iter() if node.tag in
             ('{' + NS['c'] + '}component', '{' + NS['c'] + '}item')]
            for model in models]
        main = ET.fromstring(archive.read('3D/3dmodel.model'))
        return {'meshes': meshes, 'scene_graph': transforms,
                'units': [m.attrib.get('unit', 'millimeter') for m in models],
                'objects': len(main.findall('./c:build/c:item', NS)),
                'settings': json.loads(archive.read('Metadata/project_settings.config'))}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('reference', type=Path)
    parser.add_argument('actual', type=Path)
    parser.add_argument('report', type=Path)
    args = parser.parse_args()
    reference, actual = inspect(args.reference), inspect(args.actual)
    checks = {key: reference[key] == actual[key] for key in reference}
    result = {'status': 'PASS' if all(checks.values()) else 'FAIL',
              'checks': checks, 'object_count': actual['objects'],
              'actual_sha256': hashlib.sha256(args.actual.read_bytes()).hexdigest(),
              'scope': 'Meshes, units, component/build references and transforms, object count, all project settings. UUID-only metadata excluded.'}
    args.report.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(result))
    raise SystemExit(result['status'] != 'PASS')


if __name__ == '__main__':
    main()
