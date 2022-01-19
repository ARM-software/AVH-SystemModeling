within;
package ARM "Modelica package for ARM Virtual hardware."
  // only annotations can be stored in a package.mo

  annotation(version="0.1.0",
versionDate="2022-01-12",
    Icon(graphics = {Bitmap(extent = {{-100, -100}, {100, 100}}, imageSource = "/9j/4AAQSkZJRgABAQEASABIAAD/4QDURXhpZgAASUkqAAgAAAAIAAABCQABAAAAZAAAAAEBCQAB
AAAAHwAAABoBBQABAAAAbgAAABsBBQABAAAAdgAAACgBCQABAAAAAgAAADEBAgA0AAAAfgAAADIB
AgAaAAAAsgAAACqICQABAAAAAQAAAAAAAABIAAAAAQAAAEgAAAABAAAAQ3JlYXRlZCB3aXRoIHRo
ZSBXb2xmcmFtIExhbmd1YWdlIDogd3d3LndvbGZyYW0uY29tADIwMjE6MTE6MjYgMDg6NDY6MTkr
MDE6MDAA/9sAQwAIBgYHBgUIBwcHCQkICgwUDQwLCwwZEhMPFB0aHx4dGhwcICQuJyAiLCMcHCg3
KSwwMTQ0NB8nOT04MjwuMzQy/9sAQwEJCQkMCwwYDQ0YMiEcITIyMjIyMjIyMjIyMjIyMjIyMjIy
MjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIy/8AAEQgAHwBkAwEiAAIRAQMRAf/EAB8AAAEF
AQEBAQEBAAAAAAAAAAABAgMEBQYHCAkKC//EALUQAAIBAwMCBAMFBQQEAAABfQECAwAEEQUSITFB
BhNRYQcicRQygZGhCCNCscEVUtHwJDNicoIJChYXGBkaJSYnKCkqNDU2Nzg5OkNERUZHSElKU1RV
VldYWVpjZGVmZ2hpanN0dXZ3eHl6g4SFhoeIiYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrC
w8TFxsfIycrS09TV1tfY2drh4uPk5ebn6Onq8fLz9PX29/j5+v/EAB8BAAMBAQEBAQEBAQEAAAAA
AAABAgMEBQYHCAkKC//EALURAAIBAgQEAwQHBQQEAAECdwABAgMRBAUhMQYSQVEHYXETIjKBCBRC
kaGxwQkjM1LwFWJy0QoWJDThJfEXGBkaJicoKSo1Njc4OTpDREVGR0hJSlNUVVZXWFlaY2RlZmdo
aWpzdHV2d3h5eoKDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT
1NXW19jZ2uLj5OXm5+jp6vLz9PX29/j5+v/aAAwDAQACEQMRAD8A9A+InxC/4RZFsNPVJNTlXcS3
Kwr2JHcnsPxNeRw3njPxVO8lvPqt8ynLeUzbE/AcCq/iq4m1jx1qTMSXkvGiTPYBtqj8gK+k9F0i
00PSLfTrOMJFCoXgfePdj7k81fwojdngkPjDxj4VW407Unvgs0LIq3RYPGSCA6MeRg88cVqfDPxL
rmpeN7S2vdWvbiBo5C0cszMpwhI4Jr1Hx5odtrnhG/jmjUywQtNA5HKOozx9cYP1rxr4Tf8AJQbP
/rnL/wCgGjRoNmeq/FPUb3TPB5uLC6mtpvtEa+ZC5VsHdxkV43aePfFSW89pFql5NJclVDtIzuuM
8JnoTnqOeK9a+MX/ACI5/wCvqP8Ak1cZ8FNPt7nXr+9lQNJbQqIsj7pY8ke+Bj8aFtcHuY2hWHjS
bxLp9tJNrNk9zKMzSmRflHLHng4ANegfF/V9S0fTtLbTr+5tWeVw5hkKFgFGM4r0yvJ/jj/yDNI/
67Sf+gikndjasjhLHxj4y1Sy/sWxvL+5mlkMheNmeYjAG0N1CjGePWqv9teLfCmqjz7rULW5GHMd
wzEOPdW4Ir0f4IWUS6NqV9sHnPcCHdjnaqg4/NqZ8cLdDpmlXG0b1mdN3fBUHH6VV9bCtpc5XV/i
F4o8X3UNjpKT2wKAGCyJLyNj5iSOcZzx6daxW1fxd4U1JfPutRs7j7/lzs21x7qeGFehfA+ziFjq
t7sBmaRIg3cKBnH4kj8qn+N8EZ0PTJyo8xLlkBx2K5P8hRfWwW0udj4L8UJ4p8Nw6g4SKcMYp0B4
DjGcexBB/GivHfBWoz2miyxxswUzs3Bx/CtFQ1qUmY/jfT5tE8daipBXNwbiJvVWO4Efnj8K+gfC
3iex8UaPFeWsqebtHnQ5+aN+4I9M9D3rO8ceBrXxhZofMFvfwAiGfGQR/db1H8q8YvPh94r0a6x9
mAPRZYbhQG+nIP5iq0aJ1TPYfiN4qs9B8N3dr5yG/u4mhihByw3DBYjsACfxryf4Tf8AJQbP/rnL
/wCgGr2k/CTXtRt7i71FkgbymaGPzQ7yvg7QSCQBnGSTmtvwB8PPEHh/xdbajfwQpbxpIGKzBjkq
QOB70aJBq2dF8Yv+RHP/AF9R/wAmrlvgb/x/az/1zi/m1d58RNBv/Efhg2OnIjz+ej4dwowM55P1
rD+F/g3WPC11qMmqRRos6IE2SBskE56fWlf3R9T0mvJ/jj/yDNI/67Sf+givWK4H4n+FdU8U2Wnx
aXFG7QyOz75AuAQAOv0pLcb2KPwS/wCRTvv+v5v/AEBKg+N//IC03/r5b/0Gt74ZeGtS8MaDdWmp
xoksl0ZVCOG+Xao7e4NRfE7wvqfijS7K30yON5Ipi7h3C8bcd6d/eF0Mf4If8gDUv+vof+g0/wCN
3/Itaf8A9fn/ALIa1fhj4Y1PwvpN7b6nGiSSzh1COG4247U74m+GdS8UaLaWumRo8sVx5jB3C8bS
O/1ov7wdDybwp/yCpP8Arsf5LRXWaD8OvEVhYPFPbwhzIWGJgeMD/CihvUEj/9k=
    ")}),
  Documentation(info = "<html><head></head><body><div><font size=\"4\"><b>All files in this package are covered with the following license:</b></font></div><div><br></div><div><br></div><div>Copyright (C) 2010-2022 ARM Limited or its affiliates. All rights reserved.</div><div>&nbsp;</div><div>SPDX-License-Identifier: Apache-2.0</div><div>&nbsp;</div><div>Licensed under the Apache License, Version 2.0 (the License); you may</div><div>not use this file except in compliance with the License.</div><div>You may obtain a copy of the License at</div><div><br></div><div>&nbsp;www.apache.org/licenses/LICENSE-2.0</div><div>&nbsp;</div><div>Unless required by applicable law or agreed to in writing, software</div><div>distributed under the License is distributed on an AS IS BASIS, WITHOUT</div><div>WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.</div><div>See the License for the specific language governing permissions and</div><div>limitations under the License.</div><div><br></div></body></html>"));
end ARM;
