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

#ifdef _WIN32
static char* utf8_to_str_alloc(const char* utf8) {
	size_t in_len = strlen(utf8);
	wchar_t* wcsdata;
	char* mbsdata;
	size_t mbssize, wcssize;

	wcssize = MultiByteToWideChar(CP_UTF8, 0, utf8, in_len,  NULL, 0);
	wcsdata = (wchar_t*) malloc((wcssize + 1) * sizeof(wchar_t));
	wcssize = MultiByteToWideChar(CP_UTF8, 0, utf8, in_len, wcsdata, wcssize + 1);
	wcsdata[wcssize] = 0;

	mbssize = WideCharToMultiByte(GetACP(), 0, (LPCWSTR) wcsdata, -1, NULL, 0, NULL, NULL);
	mbsdata = (char*) malloc((mbssize + 1));
	mbssize = WideCharToMultiByte(GetACP(), 0, (LPCWSTR) wcsdata, -1, mbsdata, mbssize, NULL, NULL);
	mbsdata[mbssize] = 0;
	free(wcsdata);
	return mbsdata;
}
#endif

value get_json(string url, map<string, string>& postdata) {
  value v;
  char error[256];
  string data;
  map<string, string>::iterator it;

  for (it = postdata.begin(); it != postdata.end(); it++) {
      if (data.size()) data += "&";
      data += it->first + "=";
      data += it->second;
  }

  MEMFILE* mf = memfopen();
  CURL* curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &error);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memfwrite);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, mf);
  if (data.size()) {
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  }
  if (curl_easy_perform(curl) != CURLE_OK) {
    cerr << error << endl;
  } else {
    char* ptr = mf->data;
    string err = parse(v, ptr, mf->data + mf->size);
    if (!err.empty()) cerr << err << endl;
  }
  curl_easy_cleanup(curl);
  memfclose(mf);
  return v;
}

int
main(int argc, char* argv[]) {
  map<string, string> postdata;
  value v;
  bool loop;

  postdata.clear();
  postdata["nick"] = "picojson";
  v = get_json("http://webchat.freenode.net/e/n", postdata);
  string sid = v.get<array>()[1].get<string>();

  loop = true;
  postdata.clear();
  postdata["s"] = sid;
  while (loop) {
      v = get_json("http://webchat.freenode.net/e/s", postdata);
    array a = v.get<array>();
    for (array::iterator it = a.begin(); it != a.end(); it++) {
      array line = it->get<array>();
      if (line[0].to_str() == "c") {
        if (line[1].to_str() == "443") {
            postdata["nick"] += "_";
            v = get_json("http://webchat.freenode.net/e/n", postdata);
            sid = v.get<array>()[1].get<string>();
            break;
        }
        if (line[1].to_str() == "376") {
            loop = false;
            break;
        }
      }
    }
  }

  loop = true;
  postdata.clear();
  postdata["c"] = "JOIN #picojson";
  postdata["s"] = sid;
  v = get_json("http://webchat.freenode.net/e/p", postdata);
  while (loop) {
      v = get_json("http://webchat.freenode.net/e/s", postdata);
    array a = v.get<array>();
    for (array::iterator n = a.begin(); n != a.end(); n++) {
      array line = n->get<array>();
      if (line[0].to_str() == "c") {
        string s = line[3].serialize();
#ifdef _WIN32
        char* p = utf8_to_str_alloc(s.c_str());
        cout << p;
        cout << " ";
        free((void*)p);
        cout << endl;
#else
        cout << s << endl;
#endif
      }
    }
  }
  
  return 0;
}
