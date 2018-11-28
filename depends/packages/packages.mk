packages:=boost openssl libevent
native_packages := native_ccache

qt_native_packages = native_protobuf
qt_packages = qrencode protobuf zlib

qt5_linux_packages= qt expat dbus libxcb xcb_proto libXau xproto freetype fontconfig libX11 xextproto libXext xtrans

qt_darwin_packages=qt
qt_mingw32_packages=qt

qt_linux_$(USE_LINUX_STATIC_QT5):=$(qt5_linux_packages)
qt_linux_:=$(qt46_linux_packages)
qt_linux_packages:=$(qt_linux_$(USE_LINUX_STATIC_QT5))

wallet_packages=bdb

upnp_packages=miniupnpc

darwin_native_packages = native_biplist native_ds_store native_mac_alias

ifneq ($(build_os),darwin)
darwin_native_packages += native_cctools native_cdrkit native_libdmg-hfsplus
endif
