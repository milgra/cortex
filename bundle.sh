rm -rf cortex.app
mkdir cortex.app
mkdir -p cortex.app/bin
mkdir -p cortex.app/lib
cp -R res cortex.app
ldd build/cortex | awk '{print $3}' | xargs -I {} cp {} cortex.app/lib/
rm cortex.app/lib/libc.so.*
rm cortex.app/lib/libm.so.*
cp -R build/cortex cortex.app/bin/
echo 'PTH=$(dirname $(readlink -f $0))' > cortex.app/cortex
echo "LD_LIBRARY_PATH=\$PTH/lib \$PTH/bin/cortex -r \$PTH/res" >> cortex.app/cortex
chmod +x cortex.app/cortex
tar czf cortex.app.tar.gz cortex.app
