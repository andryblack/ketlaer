#include "mpeg_out.hpp"

#include "config.h"

#include "gettext.hpp"

#ifdef benchmark_output
#include "bench.hpp"
#endif

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

void MpegOut::run()
{
#ifdef benchmark_output
  Bench bench;
#endif

  while (true) {

    drawing = false;
    new_to_draw = false;

    output_done.signal();
    output_ready.wait();
    output_ready.reset();

    drawing = true;

    UINT8 *frame = reinterpret_cast<UINT8 *>(imlib_image_get_data_for_reading_only());

#ifdef benchmark_output
    bench.start();
#endif

    iframe = mpeg_draw(frame);

    if (iframe.data[iframe.size - 1] != 0xb7) {
      /* add sequence end code */
      iframe.data[iframe.size] = 0x00;
      iframe.data[iframe.size + 1] = 0x00;
      iframe.data[iframe.size + 2] = 0x01;
      iframe.data[iframe.size + 3] = 0xb7;
      iframe.size += 4;
    }

#ifdef benchmark_output
    bench.stop();
    bench.print("libavcodec done rendering frame ( = convert + encode)");
#endif

    fwrite(iframe.data, iframe.size, 1, fpek);
  }
}

MpegOut::MpegOut()
  : initialized(-1)
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-mpeg", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-mpeg", nl_langinfo(CODESET));
#endif
}

void MpegOut::deinit()
{
  mpeg_deinit();

  if (initialized == -1)
    return;

  fclose(fpek);
}

void MpegOut::init()
{
  if (initialized != -1)
    return;

  fpek = fopen("mms.mpg", "w+");

  mpeg_init(false);

  initialized = 1;
}
