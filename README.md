# qtar

See https://www.simula.no/publications/improving-disk-io-performance-linux

This is a minimal test program writing to /tmp/z.tar without any compression.

## Usage

    sudo qtar /path/to/input-dir > /tmp/files.list
    cd /path/to/input-dir
    sudo tar zcf /tmp/a.tar.gz -T /tmp/files.list

OR

    ./qtar-tar-gz.sh /path/to/qtar /path/to/input-dir /path/to/output.tar.gz
    注意路径要用绝对路径

## 问题

1. `archive_write_set_format_ustar` 不能处理长文件名
2. `archive_write_set_format_pax` Can't translate pathname 'XXX' to UTF-8
