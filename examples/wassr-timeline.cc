#include <curl/curl.h>
#include "../picojson.h"

typedef struct {
  char* data;   // response data from server
  size_t size;  // response size of data
} MEMFILE;

MEMFILE*
memfopen() {
  MEMFILE* mf = (MEMFILE*) malloc(sizeof(MEMFILE));
  mf->data = NULL;
  mf->size = 0;
  return mf;
}

void
memfclose(MEMFILE* mf) {
  if (mf->data) free(mf->data);
  free(mf);
}

size_t
memfwrite(char* ptr, size_t size, size_t nmemb, void* stream) {
  MEMFILE* mf = (MEMFILE*) stream;
  int block = size * nmemb;
  if (!mf->data)
    mf->data = (char*) malloc(block);
  else
    mf->data = (char*) realloc(mf->data, mf->size + block);
  if (mf->data) {
    memcpy(mf->data + mf->size, ptr, block);
    mf->size += block;
  }
  return block;
}

char*
memfstrdup(MEMFILE* mf) {
  char* buf = (char*)malloc(mf->size + 1);
  memcpy(buf, mf->data, mf->size);
  buf[mf->size] = 0;
  return buf;
}

using namespace std;
using namespace picojson;

int
main(int argc, char* argv[]) {
  char error[256];

  MEMFILE* mf = memfopen();
  CURL* curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, "http://api.wassr.jp/statuses/public_timeline.json");
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &error);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memfwrite);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, mf);
  if (curl_easy_perform(curl) != CURLE_OK) {
    cerr << error << endl;
  } else {
    value v;
    string err;
    parse(v, mf->data, mf->data + mf->size, &err);
    if (err.empty()) {
      array arr = v.get<array>();
      array::iterator it;
      for (it = arr.begin(); it != arr.end(); it++) {
        object obj = it->get<object>();
        cout << obj["user_login_id"].to_str() << ": " << obj["text"].to_str() << endl;
      }
    } else {
      cerr << err << endl;
    }
  }
  curl_easy_cleanup(curl);
  memfclose(mf);

  return 0;
}
