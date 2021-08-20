# gcc添加-g
sed -i 's#ngx_compile_opt="-c "#ngx_compile_opt="-c -g "#' auto/cc/conf

./configure --prefix=./nginx