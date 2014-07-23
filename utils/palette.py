from collections import OrderedDict

flatui_palette = OrderedDict([
    ('turquoise', '#1abc9c'),
    ('emerald', '#2ecc71'),
    ('peter river', '#3498db'),
    ('amethyst', '#9b59b6'),
    ('wet asphalt', '#34495e'),
    ('green sea', '#16a085'),
    ('nephritis', '#27ae60'),
    ('belize hole', '#2980b9'),
    ('wisteria', '#8e44ad'),
    ('midnight blue', '#2c3e50'),
    ('sun flower', '#f1c40f'),
    ('carrot', '#e67e22'),
    ('alizarin', '#e74c3c'),
    ('clouds', '#ecf0f1'),
    ('concrete', '#95a5a6'),
    ('orange', '#f39c12'),
    ('pumpkin', '#d35400'),
    ('pomegranate', '#c0392b'),
    ('silver', '#bdc3c7'),
    ('asbestos', '#7f8c8d')
    ])

dawnbringer_palette = OrderedDict([
    ('black', '#140c1c'),
    ('dark red', '#442434'),
    ('dark blue', '#30346d'),
    ('dark gray', '#4e4a4e'),
    ('dark brown', '#854c30'),
    ('dark green', '#346524'),
    ('red', '#d04648'),
    ('gray', '#757161'),
    ('blue', '#597dce'),
    ('orange', '#d27d2c'),
    ('light gray', '#8595a1'),
    ('green', '#6daa2c'),
    ('beige', '#d2aa99'),
    ('light blue', '#6dc2ca'),
    ('yellow', '#dad45e'),
    ('white', '#deeed6'),
    ])

tango_palette = OrderedDict([
    ('aluminium 1', '#2e3436'),
    ('aluminium 2', '#555753'),
    ('aluminium 3', '#888a85'),
    ('aluminium 4', '#babdb6'),
    ('aluminium 5', '#d3d7cf'),
    ('aluminium 6', '#ecf0eb'),
    ('aluminium 7', '#f7f8f5'),
    ('butter 1', '#291e00'),
    ('butter 2', '#725000'),
    ('butter 3', '#c4a000'),
    ('butter 4', '#edd400'),
    ('butter 5', '#fce94f'),
    ('butter 6', '#fffc9c'),
    ('butter 7', '#feffd0'),
    ('orange 1', '#301700'),
    ('orange 2', '#8c3700'),
    ('orange 3', '#ce5c00'),
    ('orange 4', '#f57900'),
    ('orange 5', '#fcaf3e'),
    ('orange 6', '#ffd797'),
    ('orange 7', '#fff0d7'),
    ('chocolate 1', '#271700'),
    ('chocolate 2', '#503000'),
    ('chocolate 3', '#8f5902'),
    ('chocolate 4', '#c17d11'),
    ('chocolate 5', '#e9b96e'),
    ('chocolate 6', '#efd0a7'),
    ('chocolate 7', '#faf0d7'),
    ('chameleon 1', '#173000'),
    ('chameleon 2', '#2a5703'),
    ('chameleon 3', '#4e9a06'),
    ('chameleon 4', '#73d216'),
    ('chameleon 5', '#8ae234'),
    ('chameleon 6', '#b7f774'),
    ('chameleon 7', '#e4ffc7'),
    ('skyblue 1', '#00202a'),
    ('skyblue 2', '#0a3050'),
    ('skyblue 3', '#204a87'),
    ('skyblue 4', '#3465a4'),
    ('skyblue 5', '#729fcf'),
    ('skyblue 6', '#97c4f0'),
    ('skyblue 7', '#daeeff'),
    ('plum 1', '#170720'),
    ('plum 2', '#371740'),
    ('plum 3', '#5c3566'),
    ('plum 4', '#75507b'),
    ('plum 5', '#ad7fa8'),
    ('plum 6', '#e0c0e4'),
    ('plum 7', '#fce0ff'),
    ('scarletred 1', '#270000'),
    ('scarletred 2', '#600000'),
    ('scarletred 3', '#a40000'),
    ('scarletred 4', '#cc0000'),
    ('scarletred 5', '#ef2929'),
    ('scarletred 6', '#f78787'),
    ('scarletred 7', '#ffcccc')
    ])


def print_palette(pname, colors):
    print "namespace %s {" % (pname)
    for name, clr in colors.iteritems():
        print "\tstatic const Color %s(%s);" % (name.replace(' ', '_'), clr.replace('#', '0xff'))
    print "}"


print_palette('flatui', flatui_palette)
print_palette('dawnbringer', dawnbringer_palette)
print_palette('tango', tango_palette)

