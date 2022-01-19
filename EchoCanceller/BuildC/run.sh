if [ "$1" = "noecho" ]; then
    script="noEchoLinux.mos"
    name="Echo.WavEcho"
else
    script="echoLinux.mos"
    name="Echo.VHTEcho"
fi

python3.8 params.py -m $name
./$name -f $name\_init_updated.xml

if [ "$1" != "noecho" ]; then
python3.8 analyze.py -m $name
fi
