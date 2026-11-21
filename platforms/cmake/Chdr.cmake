# libchdr, vendored in third_party/libchdr (third_party/libchdr/VENDORED.md):
# the CHD reader for the player's disc image, with its bundled LZMA decoder,
# miniz and zstd. Built as upstream's CMakeLists builds it, with its defaults
# (raw sectors, subcode and hunk CRCs checked), and without the optional
# assembly decoders, so every OS compiles the same C.
set(_chdr "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libchdr")
add_library(chdr STATIC
  ${_chdr}/src/libchdr_bitstream.c ${_chdr}/src/libchdr_cdrom.c ${_chdr}/src/libchdr_chd.c
  ${_chdr}/src/libchdr_codec_avhuff.c ${_chdr}/src/libchdr_codec_cdfl.c ${_chdr}/src/libchdr_codec_cdlz.c
  ${_chdr}/src/libchdr_codec_cdzl.c ${_chdr}/src/libchdr_codec_cdzs.c ${_chdr}/src/libchdr_codec_flac.c
  ${_chdr}/src/libchdr_codec_huff.c ${_chdr}/src/libchdr_codec_lzma.c ${_chdr}/src/libchdr_codec_zlib.c
  ${_chdr}/src/libchdr_codec_zstd.c ${_chdr}/src/libchdr_flac.c ${_chdr}/src/libchdr_huffman.c
  ${_chdr}/deps/lzma-26.02/src/LzmaDec.c ${_chdr}/deps/miniz-3.1.2/miniz.c ${_chdr}/deps/zstd-1.5.7/zstddeclib.c)
target_include_directories(chdr PUBLIC ${_chdr}/include)
target_compile_definitions(chdr PRIVATE
  WANT_RAW_DATA_SECTOR=1 WANT_SUBCODE=1 VERIFY_BLOCK_CRC=1 LOWRAM_TARGET=0
  MINIZ_NO_ARCHIVE_APIS MINIZ_NO_DEFLATE_APIS MINIZ_NO_STDIO MINIZ_NO_TIME ZSTD_DISABLE_ASM)
# third-party code: its warnings are not ours to fix
if(MSVC)
  target_compile_options(chdr PRIVATE /w)
  target_compile_definitions(chdr PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(chdr PRIVATE -w)
endif()
