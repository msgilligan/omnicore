// Master Protocol transaction code

#include "base58.h"
#include "rpcserver.h"
#include "init.h"
#include "util.h"
#include "wallet.h"

#include <stdint.h>
#include <string.h>
#include <map>

#include <fstream>
#include <algorithm>

#include <vector>

#include <utility>
#include <string>

#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include "json/json_spirit_utils.h"
#include "json/json_spirit_value.h"

#include "leveldb/db.h"
#include "leveldb/write_batch.h"

#include <openssl/sha.h>

#include <boost/multiprecision/cpp_int.hpp>

using boost::multiprecision::int128_t;
using boost::multiprecision::cpp_int;
using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace json_spirit;
using namespace leveldb;

#include "mastercore.h"

using namespace mastercore;

#include "mastercore_ex.h"
#include "mastercore_tx.h"

// initial packet interpret step
int CMPTransaction::step1()
{
  if (PACKET_SIZE_CLASS_A > pkt_size)  // class A could be 19 bytes
  {
    fprintf(mp_fp, "%s() ERROR PACKET TOO SMALL; size = %d, line %d, file: %s\n", __FUNCTION__, pkt_size, __LINE__, __FILE__);
    return -(PKT_ERROR -1);
  }

  // collect version
  memcpy(&version, &pkt[0], 2);
  swapByteOrder16(version);

  // blank out version bytes in the packet
  pkt[0]=0; pkt[1]=0;
  
  memcpy(&type, &pkt[0], 4);

  swapByteOrder32(type);

  fprintf(mp_fp, "version: %d, Class %s\n", version, !multi ? "A":"B");
  fprintf(mp_fp, "\t            type: %u (%s)\n", type, c_strMastercoinType(type));

  return (type);
}

// extract Value for certain types of packets
int CMPTransaction::step2_Value()
{
  memcpy(&nValue, &pkt[8], 8);
  swapByteOrder64(nValue);

  // here we are copying nValue into nNewValue to be stored into our leveldb later: MP_txlist
  nNewValue = nValue;

  memcpy(&currency, &pkt[4], 4);
  swapByteOrder32(currency);

  fprintf(mp_fp, "\t        currency: %u (%s)\n", currency, strMPCurrency(currency).c_str());
  fprintf(mp_fp, "\t           value: %lu.%08lu\n", nValue/COIN, nValue%COIN);

  if (MAX_INT_8_BYTES < nValue)
  {
    return (PKT_ERROR -801);  // out of range
  }

  return 0;
}

// overrun check, are we beyond the end of packet?
bool CMPTransaction::isOverrun(const char *p, unsigned int line)
{
int now = (char *)p - (char *)&pkt;
bool bRet = (now > pkt_size);

    if (bRet) fprintf(mp_fp, "%s(%sline=%u):now= %u, pkt_size= %u\n", __FUNCTION__, bRet ? "OVERRUN !!! ":"", line, now, pkt_size);

    return bRet;
}

// extract Smart Property data
// RETURNS: the pointer to the next piece to be parsed
// ERROR is returns NULL and/or sets the error_code
const char *CMPTransaction::step2_SmartProperty(int &error_code)
{
const char *p = 11 + (char *)&pkt;
std::vector<std::string>spstr;
unsigned int i;
unsigned int prop_id;

  error_code = 0;

  memcpy(&ecosystem, &pkt[4], 1);
  fprintf(mp_fp, "\t       Ecosystem: %u\n", ecosystem);

  // valid values are 1 & 2
  if ((MASTERCOIN_CURRENCY_MSC != ecosystem) && (MASTERCOIN_CURRENCY_TMSC != ecosystem))
  {
    error_code = (PKT_ERROR_SP -501);
    return NULL;
  }

  prop_id = _my_sps->peekNextSPID(ecosystem);

  memcpy(&prop_type, &pkt[5], 2);
  swapByteOrder16(prop_type);

  memcpy(&prev_prop_id, &pkt[7], 4);
  swapByteOrder32(prev_prop_id);

  fprintf(mp_fp, "\t     Property ID: %u (%s)\n", prop_id, strMPCurrency(prop_id).c_str());
  fprintf(mp_fp, "\t   Property type: %u (%s)\n", prop_type, c_strPropertyType(prop_type));
  fprintf(mp_fp, "\tPrev Property ID: %u\n", prev_prop_id);

  // only 1 & 2 are valid right now
  if ((MSC_PROPERTY_TYPE_INDIVISIBLE != prop_type) && (MSC_PROPERTY_TYPE_DIVISIBLE != prop_type))
  {
    error_code = (PKT_ERROR_SP -502);
    return NULL;
  }

  for (i = 0; i<5; i++)
  {
    spstr.push_back(std::string(p));
    p += spstr.back().size() + 1;
  }

  i = 0;
  memcpy(category, spstr[i++].c_str(), sizeof(category));
  memcpy(subcategory, spstr[i++].c_str(), sizeof(subcategory));
  memcpy(name, spstr[i++].c_str(), sizeof(name));
  memcpy(url, spstr[i++].c_str(), sizeof(url));
  memcpy(data, spstr[i++].c_str(), sizeof(data));

  fprintf(mp_fp, "\t        Category: %s\n", category);
  fprintf(mp_fp, "\t     Subcategory: %s\n", subcategory);
  fprintf(mp_fp, "\t            Name: %s\n", name);
  fprintf(mp_fp, "\t             URL: %s\n", url);
  fprintf(mp_fp, "\t            Data: %s\n", data);

  if (!isTransactionTypeAllowed(block, prop_id, type, version))
  {
    error_code = (PKT_ERROR_SP -503);
    return NULL;
  }

  // name can not be NULL
  if ('\0' == name[0])
  {
    error_code = (PKT_ERROR_SP -505);
    return NULL;
  }

  if (!p) error_code = (PKT_ERROR_SP -510);

  if (isOverrun(p, __LINE__))
  {
    error_code = (PKT_ERROR_SP -800);
    return NULL;
  }

  return p;
}

int CMPTransaction::step3_sp_fixed(const char *p)
{
  if (!p) return (PKT_ERROR_SP -1);

  memcpy(&nValue, p, 8);
  swapByteOrder64(nValue);
  p += 8;

  // here we are copying nValue into nNewValue to be stored into our leveldb later: MP_txlist
  nNewValue = nValue;

  if (MSC_PROPERTY_TYPE_INDIVISIBLE == prop_type)
  {
    fprintf(mp_fp, "\t           value: %lu\n", nValue);
    if (0 == nValue) return (PKT_ERROR_SP -101);
  }
  else
  if (MSC_PROPERTY_TYPE_DIVISIBLE == prop_type)
  {
    fprintf(mp_fp, "\t           value: %lu.%08lu\n", nValue/COIN, nValue%COIN);
    if (0 == nValue) return (PKT_ERROR_SP -102);
  }

  if (MAX_INT_8_BYTES < nValue)
  {
    return (PKT_ERROR -802);  // out of range
  }

  if (isOverrun(p, __LINE__)) return (PKT_ERROR_SP -900);

  return 0;
}

int CMPTransaction::step3_sp_variable(const char *p)
{
  if (!p) return (PKT_ERROR_SP -1);

  memcpy(&currency, p, 4);  // currency desired
  swapByteOrder32(currency);
  p += 4;

  fprintf(mp_fp, "\t        currency: %u (%s)\n", currency, strMPCurrency(currency).c_str());

  memcpy(&nValue, p, 8);
  swapByteOrder64(nValue);
  p += 8;

  // here we are copying nValue into nNewValue to be stored into our leveldb later: MP_txlist
  nNewValue = nValue;

  if (MSC_PROPERTY_TYPE_INDIVISIBLE == prop_type)
  {
    fprintf(mp_fp, "\t           value: %lu\n", nValue);
    if (0 == nValue) return (PKT_ERROR_SP -201);
  }
  else
  if (MSC_PROPERTY_TYPE_DIVISIBLE == prop_type)
  {
    fprintf(mp_fp, "\t           value: %lu.%08lu\n", nValue/COIN, nValue%COIN);
    if (0 == nValue) return (PKT_ERROR_SP -202);
  }

  if (MAX_INT_8_BYTES < nValue)
  {
    return (PKT_ERROR -803);  // out of range
  }

  memcpy(&deadline, p, 8);
  swapByteOrder64(deadline);
  p += 8;
  fprintf(mp_fp, "\t        Deadline: %s (%lX)\n", DateTimeStrFormat("%Y-%m-%d %H:%M:%S", deadline).c_str(), deadline);

  if (!deadline) return (PKT_ERROR_SP -203);  // deadline cannot be 0

  // deadline can not be smaller than the timestamp of the current block
  if (deadline < (uint64_t)blockTime) return (PKT_ERROR_SP -204);

  memcpy(&early_bird, p++, 1);
  fprintf(mp_fp, "\tEarly Bird Bonus: %u\n", early_bird);

  memcpy(&percentage, p++, 1);
  fprintf(mp_fp, "\t      Percentage: %u\n", percentage);

  if (isOverrun(p, __LINE__)) return (PKT_ERROR_SP -765);

  return 0;
}

void CMPTransaction::printInfo(FILE *fp)
{
  fprintf(fp, "BLOCK: %d txid: %s, Block Time: %s\n", block, txid.GetHex().c_str(), DateTimeStrFormat("%Y-%m-%d %H:%M:%S", blockTime).c_str());
  fprintf(fp, "sender: %s\n", sender.c_str());
}

