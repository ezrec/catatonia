//
// Copyright (C) 2017, Jason S. McMullan <jason.mcmullan@gmail.com>
// All rights reserved.
//
// Licensed under the MIT License:
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//

nema_shaft_d=5;
nema_shaft_l=14;

auger_shaft = 40;
auger_length = 70;
auger_d = 50;
auger_width = auger_d / 14;
auger_core = nema_shaft_d + 2*3;
auger_slices = 4;

// grub screw
grub_d = 3;
tolerance = 0.125;

// Maximum edge-to-edge of the auger blades
auger_clearance=15;

wall = 4;
fn=100;

plated = true;

module auger_spiral(arc=60)
{
    clearance = auger_clearance*2;
    linear_extrude(height = auger_length, center = false, convexity = 10, twist = arc * (auger_length / clearance), $fn = fn)
    {
        intersection() {
            translate([0, -auger_width/2]) square([auger_d, auger_width]);
            circle(d=auger_d);
        }
    }
}
    
module auger()
{
    // Separation plate from the motor
    for (i = [0:auger_slices-1]) {
        clearance = auger_clearance*2;
        arc = 360/auger_slices;
        rotate([0, 0, arc*i]) {
            auger_spiral(arc);
            * for (a = [0:arc]) {
                angle = arc - a;
                translate([0, 0, clearance/arc*a])
                linear_extrude(height = clearance/arc + 0.01, center=false)
                {
                    intersection() {
                        polygon([[0,0], [auger_d/2, 0], [auger_d/2, sin(angle)*auger_d/2], [cos(arc)*auger_d/2, sin(angle)*auger_d/2]]);
                        circle(d=auger_d - tolerance*4);
                    }
                }
            }
        }
    }
    cylinder(d1=auger_d+2, d2=auger_d, h = 2, $fn=fn);
    
    // Auger core
    cylinder(d1=auger_d, d2=auger_core, h = auger_length, $fn=fn);
    
    // Motor attachment
    translate([0, 0, -auger_shaft])
    {
        difference()
        {
            union()
            {
                cylinder(d=auger_core, h = auger_shaft/2 + 0.1, $fn=fn);
                translate([0, 0, auger_shaft/2])
                    cylinder(d1=auger_core, d2=auger_d/2, h = auger_shaft/2 + 0.1, $fn=fn);
            }
            translate([0, 0, wall/2 + grub_d/2]) {
                rotate([0, 90, 0]) cylinder(d=grub_d, h = auger_core, $fn=fn);
            }
            translate([0, 0, nema_shaft_l - wall/2 - grub_d/2]) {
                rotate([0, 90, 0]) cylinder(d=grub_d, h = auger_core, $fn=fn);
            }
            
            translate([0, 0, -0.01]) {
                cylinder(d = nema_shaft_d+tolerance*2, h=nema_shaft_l+2, $fn=fn);
            }
        }
    }
}

module auger_cap_plate()
{
    difference()
    {
        union()
        {
            cylinder(d1=auger_d+2, d2=auger_d, h = 2, $fn=fn);
            cylinder(d=auger_d, d2=auger_d, h = 2 + 1, $fn=fn);
        }
        translate([0, -(auger_d+5)/2, -1]) cube([auger_d+5, auger_d+5, 1+3+1]);
    }
}

if (!plated)
{
    rotate([0, 0, 30]) auger();
    # difference()
    {
        cylinder(d=auger_d + wall*2, h=auger_length);
        translate([0, 0, -1]) cylinder(d=auger_d, h=auger_length+2);
        translate([0, 0, auger_length - (auger_clearance + auger_d/2)]) rotate([0, 90, 0]) cylinder(d=auger_d, h = 100);
    }

    translate([0, 0, auger_length+2]) rotate([0, 180, 270]) auger_cap_plate();
}
else
{
    rotate([0, 180, 0]) translate([0, 0, -auger_length]) auger();
    translate([0, auger_d+5, 0]) auger_cap_plate();
}

// vim: set shiftwidth=4 expandtab: //
