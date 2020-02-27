# LVHA256
> The LV Hashing Algorithm


This is a hashing algorithm of 256 bits, created by myself, as an exercise.
It was not audited yet, so the security can't be guaranted. Don't use it for a
real application where the security is essencial (or any application at all).

![](header.png)



    Usage: lvha256sum [ -s <text> ] <filename>

    Example: $ ./lvha256sum test_files/input_file.1
             a021bb7deffff8ffe0368ab784efbb66783c3ff7f3f3faf9b467be89f9ffb5fd

             $ ./lvha256sum -s hello
             fb37665ffe1fefef28e0c7c4bfafeb3f68fdf8bffdbb2ff2eb0e1e459f7ff71f

## Author

Lucas V. Araujo – lucas.vieira.ar@disroot.org

Distributed under the GNU GPL-3.0+ license. See ``LICENSE`` for more information.

[GitHub](https://github.com/LvMalware/lvha256)

## Contributing
    Everyone's cooperation is welcome.
If you are a cryptanalyst (or have some knowledge of this area) I would be very grateful if you could perform some tests with this algorithm and share your findings.

1. Fork it (<https://github.com/LvMalware/lvha256/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request
