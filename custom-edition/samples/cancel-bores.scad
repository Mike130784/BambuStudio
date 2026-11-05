// Own cancellation fixture: many small bores, no external dependencies.
width = 30;
depth = 20;
height = 5;
difference() {
    cube([width, depth, height]);
    for (x = [1:24], y = [1:16])
        translate([width*x/25, depth*y/17, -1])
            cylinder(h=height+2, r=0.15, $fn=24);
}
