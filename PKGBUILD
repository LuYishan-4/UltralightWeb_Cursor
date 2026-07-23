pkgname=ultralightwebcursor-git
pkgver=r34.7b7b5ec
pkgrel=1

pkgdesc="Ultralight HTML cursor effect for KDE Plasma"
arch=('x86_64')

url="https://github.com/LuYishan-4/UltralightWeb_Cursor"
license=('MIT')


depends=(
    'qt6-base'
    'kwin'
)

makedepends=(
    'git'
    'cmake'
    'ninja'
    'extra-cmake-modules'
    'p7zip'
)


source=(
    "git+https://github.com/LuYishan-4/UltralightWeb_Cursor.git"
)

sha256sums=(
    'SKIP'
)



pkgver()
{
    cd "${srcdir}/UltralightWeb_Cursor"

    printf "r%s.%s" \
        "$(git rev-list --count HEAD)" \
        "$(git rev-parse --short HEAD)"
}



prepare()
{
    cd "${srcdir}/UltralightWeb_Cursor"


    if [ ! -d "sdk/ultralight-free-sdk-1.4.0-linux-x64" ]; then

        echo "Extracting Ultralight SDK..."

        7z x \
            "sdk/ultralight-free-sdk-1.4.0-linux-x64.7z" \
            -osdk

    fi
}



build()
{
    cd "${srcdir}/UltralightWeb_Cursor"


    cmake \
        -B build \
        -S . \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr


    cmake --build build
}



package()
{
    cd "${srcdir}/UltralightWeb_Cursor"


    DESTDIR="${pkgdir}" \
        cmake --install build
}
