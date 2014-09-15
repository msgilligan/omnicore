// global Master Protocol header file
// globals consts (DEFINEs for now) should be here
//
// for now (?) class declarations go here -- work in progress; probably will get pulled out into a separate file, note: some declarations are still in the .cpp file

#ifndef _MASTERCOIN
#define _MASTERCOIN 1

#include "netbase.h"
#include "protocol.h"

#define LOG_FILENAME    "mastercore.log"
#define INFO_FILENAME   "mastercore_crowdsales.log"
#define OWNERS_FILENAME "mastercore_owners.log"

int const MAX_STATE_HISTORY = 50;

#define TEST_ECO_PROPERTY_1 (0x80000003UL)

// could probably also use: int64_t maxInt64 = std::numeric_limits<int64_t>::max();
// maximum numeric values from the spec: 
#define MAX_INT_8_BYTES (9223372036854775807UL)

// what should've been in the Exodus address for this block if none were spent
#define DEV_MSC_BLOCK_290629 (1743358325718)

#define SP_STRING_FIELD_LEN 256

// in Mastercoin Satoshis (Willetts)
#define TRANSFER_FEE_PER_OWNER  (1)

// some boost formats
#define FORMAT_BOOST_TXINDEXKEY "index-tx-%s"
#define FORMAT_BOOST_SPKEY      "sp-%d"

// Master Protocol Transaction (Packet) Version
#define MP_TX_PKT_V0  0
#define MP_TX_PKT_V1  1

// Maximum outputs per BTC Transaction
#define MAX_BTC_OUTPUTS 16

// TODO: clean up is needed for pre-production #DEFINEs , consts & alike belong in header files (classes)
#define MAX_SHA256_OBFUSCATION_TIMES  255

#define PACKET_SIZE_CLASS_A 19
#define PACKET_SIZE         31
#define MAX_PACKETS         64

#define GOOD_PRECISION  (1e10)

// Transaction types, from the spec
enum TransactionType {
  MSC_TYPE_SIMPLE_SEND              =  0,
  MSC_TYPE_RESTRICTED_SEND          =  2,
  MSC_TYPE_SEND_TO_OWNERS           =  3,
  MSC_TYPE_AUTOMATIC_DISPENSARY     = 15,
  MSC_TYPE_TRADE_OFFER              = 20,
  MSC_TYPE_METADEX                  = 21,
  MSC_TYPE_ACCEPT_OFFER_BTC         = 22,
  MSC_TYPE_OFFER_ACCEPT_A_BET       = 40,
  MSC_TYPE_CREATE_PROPERTY_FIXED    = 50,
  MSC_TYPE_CREATE_PROPERTY_VARIABLE = 51,
  MSC_TYPE_PROMOTE_PROPERTY         = 52,
  MSC_TYPE_CLOSE_CROWDSALE          = 53,
  MSC_TYPE_CREATE_PROPERTY_MANUAL   = 54,
  MSC_TYPE_GRANT_PROPERTY_TOKENS    = 55,
  MSC_TYPE_REVOKE_PROPERTY_TOKENS   = 56,
};

#define MSC_PROPERTY_TYPE_INDIVISIBLE             1
#define MSC_PROPERTY_TYPE_DIVISIBLE               2
#define MSC_PROPERTY_TYPE_INDIVISIBLE_REPLACING   65
#define MSC_PROPERTY_TYPE_DIVISIBLE_REPLACING     66
#define MSC_PROPERTY_TYPE_INDIVISIBLE_APPENDING   129
#define MSC_PROPERTY_TYPE_DIVISIBLE_APPENDING     130

// block height (MainNet) with which the corresponding transaction is considered valid, per spec
enum BLOCKHEIGHTRESTRICTIONS {
// starting block for parsing on TestNet
  START_TESTNET_BLOCK=263000,
  START_REGTEST_BLOCK=5,
  MONEYMAN_REGTEST_BLOCK= 101, // new address to assign MSC & TMSC on RegTest
  MONEYMAN_TESTNET_BLOCK= 270775, // new address to assign MSC & TMSC on TestNet
  POST_EXODUS_BLOCK = 255366,
  MSC_DEX_BLOCK     = 290630,
  MSC_SP_BLOCK      = 297110,
  GENESIS_BLOCK     = 249498,
  LAST_EXODUS_BLOCK = 255365,
  MSC_STO_BLOCK     = 999999,
  MSC_METADEX_BLOCK = 999999,
  MSC_BET_BLOCK     = 999999,
  MSC_MANUALSP_BLOCK = 999999,
  P2SH_BLOCK        = 999999,
};

enum FILETYPES {
  FILETYPE_BALANCES = 0,
  FILETYPE_OFFERS,
  FILETYPE_ACCEPTS,
  FILETYPE_GLOBALS,
  FILETYPE_CROWDSALES,
  NUM_FILETYPES
};

#define PKT_RETURN_OFFER    (1000)
// #define PKT_RETURN_ACCEPT   (2000)

#define PKT_ERROR             ( -9000)
#define DEX_ERROR_SELLOFFER   (-10000)
#define DEX_ERROR_ACCEPT      (-20000)
#define DEX_ERROR_PAYMENT     (-30000)
// Smart Properties
#define PKT_ERROR_SP          (-40000)
// Send To Owners
#define PKT_ERROR_STO         (-50000)
#define PKT_ERROR_SEND        (-60000)
#define PKT_ERROR_TRADEOFFER  (-70000)
#define PKT_ERROR_METADEX     (-80000)
#define METADEX_ERROR         (-81000)
#define PKT_ERROR_TOKENS      (-82000)

#define CLASSB_SEND_ERROR     (-200)

#define MASTERCOIN_CURRENCY_BTC   0
#define MASTERCOIN_CURRENCY_MSC   1
#define MASTERCOIN_CURRENCY_TMSC  2

// forward declarations
string FormatDivisibleMP(int64_t n, bool fSign = false);

const std::string ExodusAddress();

extern FILE *mp_fp;

extern int msc_debug_dex;

extern CCriticalSection cs_tally;

enum TallyType { MONEY = 0, SELLOFFER_RESERVE = 1, ACCEPT_RESERVE = 2, TALLY_TYPE_COUNT };

class CMPTally
{
private:
typedef struct
{
  uint64_t balance[TALLY_TYPE_COUNT];
} BalanceRecord;

  typedef std::map<unsigned int, BalanceRecord> TokenMap;
  TokenMap mp_token;
  TokenMap::iterator my_it;

  bool propertyExists(unsigned int which_currency) const
  {
  const TokenMap::const_iterator it = mp_token.find(which_currency);

    return (it != mp_token.end());
  }

public:

  unsigned int init()
  {
  unsigned int ret = 0;

    my_it = mp_token.begin();
    if (my_it != mp_token.end()) ret = my_it->first;

    return ret;
  }

  unsigned int next()
  {
  unsigned int ret;

    if (my_it == mp_token.end()) return 0;

    ret = my_it->first;

    ++my_it;

    return ret;
  }

  bool updateMoney(unsigned int which_currency, int64_t amount, TallyType ttype)
  {
  bool bRet = false;
  int64_t now64;

    LOCK(cs_tally);

    now64 = mp_token[which_currency].balance[ttype];

    if (0>(now64 + amount))
    {
    }
    else
    {
      now64 += amount;
      mp_token[which_currency].balance[ttype] = now64;

      bRet = true;
    }

    return bRet;
  }

  // the constructor -- create an empty tally for an address
  CMPTally()
  {
    my_it = mp_token.begin();
  }

  uint64_t print(int which_currency = MASTERCOIN_CURRENCY_MSC, bool bDivisible = true)
  {
  uint64_t money = 0;
  uint64_t so_r = 0;
  uint64_t a_r = 0;

    if (propertyExists(which_currency))
    {
      money = mp_token[which_currency].balance[MONEY];
      so_r = mp_token[which_currency].balance[SELLOFFER_RESERVE];
      a_r = mp_token[which_currency].balance[ACCEPT_RESERVE];
    }

    if (bDivisible)
    {
      printf("%22s [SO_RESERVE= %22s , ACCEPT_RESERVE= %22s ]\n",
       FormatDivisibleMP(money).c_str(), FormatDivisibleMP(so_r).c_str(), FormatDivisibleMP(a_r).c_str());
    }
    else
    {
      printf("%14lu [SO_RESERVE= %14lu , ACCEPT_RESERVE= %14lu ]\n", money, so_r, a_r);
    }

    return (money + so_r + a_r);
  }

  uint64_t getMoney(unsigned int which_currency, TallyType ttype)
  {
  uint64_t ret64 = 0;

    LOCK(cs_tally);

    if (propertyExists(which_currency)) ret64 = mp_token[which_currency].balance[ttype];

    return ret64;
  }
};

/* leveldb-based storage for the list of ALL Master Protocol TXIDs (key) with validity bit & other misc data as value */
class CMPTxList
{
protected:
    // database options used
    leveldb::Options options;

    // options used when reading from the database
    leveldb::ReadOptions readoptions;

    // options used when iterating over values of the database
    leveldb::ReadOptions iteroptions;

    // options used when writing to the database
    leveldb::WriteOptions writeoptions;

    // options used when sync writing to the database
    leveldb::WriteOptions syncoptions;

    // the database itself
    leveldb::DB *pdb;

    // statistics
    unsigned int nWritten;
    unsigned int nRead;

public:
    CMPTxList(const boost::filesystem::path &path, size_t nCacheSize, bool fMemory, bool fWipe):nWritten(0),nRead(0)
    {
      options.paranoid_checks = true;
      options.create_if_missing = true;

      readoptions.verify_checksums = true;
      iteroptions.verify_checksums = true;
      iteroptions.fill_cache = false;
      syncoptions.sync = true;

      leveldb::Status status = leveldb::DB::Open(options, path.string(), &pdb);

      printf("%s(): %s, line %d, file: %s\n", __FUNCTION__, status.ToString().c_str(), __LINE__, __FILE__);
    }

    ~CMPTxList()
    {
      delete pdb;
      pdb = NULL;
    }

    void recordTX(const uint256 &txid, bool fValid, int nBlock, unsigned int type, uint64_t nValue);
    void recordPaymentTX(const uint256 &txid, bool fValid, int nBlock, unsigned int vout, unsigned int propertyId, uint64_t nValue, string buyer, string seller);

    int getNumberOfPurchases(const uint256 txid);
    bool getPurchaseDetails(const uint256 txid, int purchaseNumber, string *buyer, string *seller, uint64_t *vout, uint64_t *propertyId, uint64_t *nValue);

    bool exists(const uint256 &txid);
    bool getTX(const uint256 &txid, string &value);

    void printStats();
    void printAll();

    bool isMPinBlockRange(int, int, bool);
};

// live crowdsales are these objects in a map
class CMPCrowd
{
private:
  unsigned int propertyId;

  uint64_t nValue;

  unsigned int currency_desired;
  uint64_t deadline;
  unsigned char early_bird;
  unsigned char percentage;

  uint64_t u_created;
  uint64_t i_created;

  uint256 txid;  // NOTE: not persisted as it doesnt seem used

  std::map<std::string, std::vector<uint64_t> > txFundraiserData;  // schema is 'txid:amtSent:deadlineUnix:userIssuedTokens:IssuerIssuedTokens;'
public:
  CMPCrowd():propertyId(0),nValue(0),currency_desired(0),deadline(0),early_bird(0),percentage(0),u_created(0),i_created(0)
  {
  }

  CMPCrowd(unsigned int pid, uint64_t nv, unsigned int cd, uint64_t dl, unsigned char eb, unsigned char per, uint64_t uct, uint64_t ict):
   propertyId(pid),nValue(nv),currency_desired(cd),deadline(dl),early_bird(eb),percentage(per),u_created(uct),i_created(ict)
  {
  }

  unsigned int getPropertyId() const { return propertyId; }

  uint64_t getDeadline() const { return deadline; }
  uint64_t getCurrDes() const { return currency_desired; }

  void incTokensUserCreated(uint64_t amount) { u_created += amount; }
  void incTokensIssuerCreated(uint64_t amount) { i_created += amount; }

  uint64_t getUserCreated() const { return u_created; }
  uint64_t getIssuerCreated() const { return i_created; }

  void insertDatabase(std::string txhash, std::vector<uint64_t> txdata ) { txFundraiserData.insert(std::make_pair<std::string, std::vector<uint64_t>& >(txhash,txdata)); }
  std::map<std::string, std::vector<uint64_t> > getDatabase() const { return txFundraiserData; }

  void print(const string & address, FILE *fp = stdout) const
  {
    fprintf(fp, "%34s : id=%u=%X; curr=%u, value= %lu, deadline: %s (%lX)\n", address.c_str(), propertyId, propertyId,
     currency_desired, nValue, DateTimeStrFormat("%Y-%m-%d %H:%M:%S", deadline).c_str(), deadline);
  }

  void saveCrowdSale(ofstream &file, SHA256_CTX *shaCtx, string const &addr) const
  {
    // compose the outputline
    // addr,propertyId,nValue,currency_desired,deadline,early_bird,percentage,created,mined
    string lineOut = (boost::format("%s,%d,%d,%d,%d,%d,%d,%d,%d")
      % addr
      % propertyId
      % nValue
      % currency_desired
      % deadline
      % (int)early_bird
      % (int)percentage
      % u_created
      % i_created ).str();

    // append N pairs of address=nValue;blockTime for the database
    std::map<std::string, std::vector<uint64_t> >::const_iterator iter;
    for (iter = txFundraiserData.begin(); iter != txFundraiserData.end(); ++iter) {
      lineOut.append((boost::format(",%s=") % (*iter).first).str());
      std::vector<uint64_t> const &vals = (*iter).second;

      std::vector<uint64_t>::const_iterator valIter;
      for (valIter = vals.begin(); valIter != vals.end(); ++valIter) {
        if (valIter != vals.begin()) {
          lineOut.append(";");
        }

        lineOut.append((boost::format("%d") % (*valIter)).str());
      }
    }

    // add the line to the hash
    SHA256_Update(shaCtx, lineOut.c_str(), lineOut.length());

    // write the line
    file << lineOut << endl;
  }
};  // end of CMPCrowd class

class CMPSPInfo
{
public:
  struct Entry {
    // common SP data
    string issuer;
    unsigned short prop_type;
    unsigned int prev_prop_id;
    string category;
    string subcategory;
    string name;
    string url;
    string data;
    uint64_t num_tokens;

    // Crowdsale generated SP
    unsigned int currency_desired;
    uint64_t deadline;
    unsigned char early_bird;
    unsigned char percentage;

    //We need this. Closedearly states if the SP was a crowdsale and closed due to MAXTOKENS or CLOSE command
    bool close_early;
    bool max_tokens;
    uint64_t timeclosed;
    uint256 txid_close;

    // other information
    uint256 txid;
    uint256 creation_block;
    uint256 update_block;
    bool fixed;
    bool manual;

    // for crowdsale properties, schema is 'txid:amtSent:deadlineUnix:userIssuedTokens:IssuerIssuedTokens;'
    // for manual properties, schema is 'txid:grantAmount:revokeAmount;'
    std::map<std::string, std::vector<uint64_t> > historicalData;
    
    Entry()
    : issuer()
    , prop_type(0)
    , prev_prop_id(0)
    , category()
    , subcategory()
    , name()
    , url()
    , data()
    , num_tokens(0)
    , currency_desired(0)
    , deadline(0)
    , early_bird(0)
    , percentage(0)
    , close_early(0)
    , max_tokens(0)
    , timeclosed(0)
    , txid_close()
    , txid()
    , creation_block()
    , update_block()
    , fixed(false)
    , manual(false)
    , historicalData()
    {
    }

    Object toJSON() const
    {
      Object spInfo;
      spInfo.push_back(Pair("issuer", issuer));
      spInfo.push_back(Pair("prop_type", prop_type));
      spInfo.push_back(Pair("prev_prop_id", (uint64_t)prev_prop_id));
      spInfo.push_back(Pair("category", category));
      spInfo.push_back(Pair("subcategory", subcategory));
      spInfo.push_back(Pair("name", name));
      spInfo.push_back(Pair("url", url));
      spInfo.push_back(Pair("data", data));
      spInfo.push_back(Pair("fixed", fixed));
      spInfo.push_back(Pair("manual", manual));

      spInfo.push_back(Pair("num_tokens", (boost::format("%d") % num_tokens).str()));
      if (false == fixed && false == manual) {
        spInfo.push_back(Pair("currency_desired", (uint64_t)currency_desired));
        spInfo.push_back(Pair("deadline", (boost::format("%d") % deadline).str()));
        spInfo.push_back(Pair("early_bird", (int)early_bird));
        spInfo.push_back(Pair("percentage", (int)percentage));

        spInfo.push_back(Pair("close_early", (int)close_early));
        spInfo.push_back(Pair("max_tokens", (int)max_tokens));
        spInfo.push_back(Pair("timeclosed", (boost::format("%d") % timeclosed).str()));
        spInfo.push_back(Pair("txid_close", (boost::format("%s") % txid_close.ToString()).str()));
      }

      //Initialize values
      std::map<std::string, std::vector<uint64_t> >::const_iterator it;
      
      std::string values_long = "";
      std::string values = "";

      //fprintf(mp_fp,"\ncrowdsale started to save, size of db %ld", database.size());

      //Iterate through fundraiser data, serializing it with txid:val:val:val:val;
      bool crowdsale = !fixed && !manual;
      for(it = historicalData.begin(); it != historicalData.end(); it++) {
         values += it->first.c_str();
         if (crowdsale) {
           values += ":" + boost::lexical_cast<std::string>(it->second.at(0));
           values += ":" + boost::lexical_cast<std::string>(it->second.at(1));
           values += ":" + boost::lexical_cast<std::string>(it->second.at(2));
           values += ":" + boost::lexical_cast<std::string>(it->second.at(3));
         } else if (manual) {
           values += ":" + boost::lexical_cast<std::string>(it->second.at(0));
           values += ":" + boost::lexical_cast<std::string>(it->second.at(1));
         }
         values += ";";
         values_long += values;
      }
      //fprintf(mp_fp,"\ncrowdsale saved %s", values_long.c_str());

      spInfo.push_back(Pair("historicalData", values_long));
      spInfo.push_back(Pair("txid", (boost::format("%s") % txid.ToString()).str()));
      spInfo.push_back(Pair("creation_block", (boost::format("%s") % creation_block.ToString()).str()));
      spInfo.push_back(Pair("update_block", (boost::format("%s") % update_block.ToString()).str()));
      return spInfo;
    }

    void fromJSON(Object const &json)
    {
      unsigned int idx = 0;
      issuer = json[idx++].value_.get_str();
      prop_type = (unsigned short)json[idx++].value_.get_int();
      prev_prop_id = (unsigned int)json[idx++].value_.get_uint64();
      category = json[idx++].value_.get_str();
      subcategory = json[idx++].value_.get_str();
      name = json[idx++].value_.get_str();
      url = json[idx++].value_.get_str();
      data = json[idx++].value_.get_str();
      fixed = json[idx++].value_.get_bool();
      manual = json[idx++].value_.get_bool();

      num_tokens = boost::lexical_cast<uint64_t>(json[idx++].value_.get_str());
      if (false == fixed && false == manual) {
        currency_desired = (unsigned int)json[idx++].value_.get_uint64();
        deadline = boost::lexical_cast<uint64_t>(json[idx++].value_.get_str());
        early_bird = (unsigned char)json[idx++].value_.get_int();
        percentage = (unsigned char)json[idx++].value_.get_int();

        close_early = (unsigned char)json[idx++].value_.get_int();
        max_tokens = (unsigned char)json[idx++].value_.get_int();
        timeclosed = boost::lexical_cast<uint64_t>(json[idx++].value_.get_str());
        txid_close = uint256(json[idx++].value_.get_str());
      }

      //reconstruct database
      std::string longstr = json[idx++].value_.get_str();
      
      //fprintf(mp_fp,"\nDESERIALIZE GO ----> %s" ,longstr.c_str() );
      
      std::vector<std::string> strngs_vec;
      
      //split serialized form up
      boost::split(strngs_vec, longstr, boost::is_any_of(";"));

      //fprintf(mp_fp,"\nDATABASE PRE-DESERIALIZE SUCCESS, %ld, %s" ,strngs_vec.size(), strngs_vec[0].c_str());
      
      //Go through and deserialize the database
      bool crowdsale = !fixed && !manual;
      for(std::vector<std::string>::size_type i = 0; i != strngs_vec.size(); i++) {
        if (strngs_vec[i].empty()) {
          continue;
        }
        
        std::vector<std::string> str_split_vec;
        boost::split(str_split_vec, strngs_vec[i], boost::is_any_of(":"));

        std::vector<uint64_t> txDataVec;

        if ( crowdsale && str_split_vec.size() == 5) {
          //fprintf(mp_fp,"\n Deserialized values: %s, %s %s %s %s", str_split_vec.at(0).c_str(), str_split_vec.at(1).c_str(), str_split_vec.at(2).c_str(), str_split_vec.at(3).c_str(), str_split_vec.at(4).c_str());
          txDataVec.push_back(boost::lexical_cast<uint64_t>( str_split_vec.at(1) ));
          txDataVec.push_back(boost::lexical_cast<uint64_t>( str_split_vec.at(2) ));
          txDataVec.push_back(boost::lexical_cast<uint64_t>( str_split_vec.at(3) ));
          txDataVec.push_back(boost::lexical_cast<uint64_t>( str_split_vec.at(4) ));
        } else if (manual && str_split_vec.size() == 3) {
          txDataVec.push_back(boost::lexical_cast<uint64_t>( str_split_vec.at(1) ));
          txDataVec.push_back(boost::lexical_cast<uint64_t>( str_split_vec.at(2) ));
        }

        historicalData.insert(std::make_pair( str_split_vec.at(0), txDataVec ) ) ;
      }
      //fprintf(mp_fp,"\nDATABASE DESERIALIZE SUCCESS %lu", database.size());
      txid = uint256(json[idx++].value_.get_str());
      creation_block = uint256(json[idx++].value_.get_str());
      update_block = uint256(json[idx++].value_.get_str());
    }

    bool isDivisible() const
    {
      switch (prop_type)
      {
        case MSC_PROPERTY_TYPE_DIVISIBLE:
        case MSC_PROPERTY_TYPE_DIVISIBLE_REPLACING:
        case MSC_PROPERTY_TYPE_DIVISIBLE_APPENDING:
          return true;
      }
      return false;
    }

    void print() const
    {
      printf("%s:%s(Fixed=%s,Divisible=%s):%lu:%s/%s, %s %s\n",
        issuer.c_str(),
        name.c_str(),
        fixed ? "Yes":"No",
        isDivisible() ? "Yes":"No",
        num_tokens,
        category.c_str(), subcategory.c_str(), url.c_str(), data.c_str());
    }

  };

private:
  leveldb::DB *pDb;
  boost::filesystem::path const path;

  // implied version of msc and tmsc so they don't hit the leveldb
  Entry implied_msc;
  Entry implied_tmsc;

  unsigned int next_spid;
  unsigned int next_test_spid;

  void openDB() {
    leveldb::Options options;
    options.paranoid_checks = true;
    options.create_if_missing = true;

    leveldb::Status s = leveldb::DB::Open(options, path.string(), &pDb);

     if (false == s.ok()) {
       printf("Failed to create or read LevelDB for Smart Property at %s", path.c_str());
     }
  }

  void closeDB() {
    delete pDb;
    pDb = NULL;
  }

public:

  CMPSPInfo(const boost::filesystem::path &_path)
    : path(_path)
  {
    openDB();

    // special cases for constant SPs MSC and TMSC
    implied_msc.issuer = ExodusAddress();
    implied_msc.prop_type = MSC_PROPERTY_TYPE_DIVISIBLE;
    implied_msc.num_tokens = 700000;
    implied_msc.category = "N/A";
    implied_msc.subcategory = "N/A";
    implied_msc.name = "MasterCoin";
    implied_msc.url = "www.mastercoin.org";
    implied_msc.data = "***data***";
    implied_tmsc.issuer = ExodusAddress();
    implied_tmsc.prop_type = MSC_PROPERTY_TYPE_DIVISIBLE;
    implied_tmsc.num_tokens = 700000;
    implied_tmsc.category = "N/A";
    implied_tmsc.subcategory = "N/A";
    implied_tmsc.name = "Test MasterCoin";
    implied_tmsc.url = "www.mastercoin.org";
    implied_tmsc.data = "***data***";

    init();
  }

  ~CMPSPInfo()
  {
    closeDB();
  }

  void init(unsigned int nextSPID = 0x3UL, unsigned int nextTestSPID = TEST_ECO_PROPERTY_1)
  {
    next_spid = nextSPID;
    next_test_spid = nextTestSPID;
  }

  void clear() {
    closeDB();
    leveldb::DestroyDB(path.string(), leveldb::Options());
    openDB();
    init();
  }

  unsigned int peekNextSPID(unsigned char ecosystem)
  {
    unsigned int nextId = 0;

    switch(ecosystem) {
    case MASTERCOIN_CURRENCY_MSC: // mastercoin ecosystem, MSC: 1, TMSC: 2, First available SP = 3
      nextId = next_spid;
      break;
    case MASTERCOIN_CURRENCY_TMSC: // Test MSC ecosystem, same as above with high bit set
      nextId = next_test_spid;
      break;
    default: // non standard ecosystem, ID's start at 0
      nextId = 0;
    }

    return nextId;
  }

  unsigned int updateSP(unsigned int propertyID, Entry const &info);
  unsigned int putSP(unsigned char ecosystem, Entry const &info);

  bool getSP(unsigned int spid, Entry &info)
  {
    // special cases for constant SPs MSC and TMSC
    if (MASTERCOIN_CURRENCY_MSC == spid) {
      info = implied_msc;
      return true;
    } else if (MASTERCOIN_CURRENCY_TMSC == spid) {
      info = implied_tmsc;
      return true;
    }

    leveldb::ReadOptions readOpts;
    readOpts.fill_cache = true;

    string spKey = (boost::format(FORMAT_BOOST_SPKEY) % spid).str();
    string spInfoStr;
    if (false == pDb->Get(readOpts, spKey, &spInfoStr).ok()) {
      return false;
    }

    // parse the encoded json, failing if it doesnt parse or is an object
    Value spInfoVal;
    if (false == read_string(spInfoStr, spInfoVal) || spInfoVal.type() != obj_type ) {
      return false;
    }

    // transfer to the Entry structure
    Object &spInfo = spInfoVal.get_obj();
    info.fromJSON(spInfo);
    return true;
  }

  bool hasSP(unsigned int spid)
  {
    // special cases for constant SPs MSC and TMSC
    if (MASTERCOIN_CURRENCY_MSC == spid || MASTERCOIN_CURRENCY_TMSC == spid) {
      return true;
    }

    leveldb::ReadOptions readOpts;
    readOpts.fill_cache = true;

    string spKey = (boost::format(FORMAT_BOOST_SPKEY) % spid).str();
    leveldb::Iterator *iter = pDb->NewIterator(readOpts);
    iter->Seek(spKey);
    bool res = (iter->Valid() && iter->key().compare(spKey) == 0);
    // clean up the iterator
    delete iter;

    return res;
  }

  unsigned int findSPByTX(uint256 const &txid)
  {
    unsigned int res = 0;
    leveldb::ReadOptions readOpts;
    readOpts.fill_cache = true;

    string txIndexKey = (boost::format(FORMAT_BOOST_TXINDEXKEY) % txid.ToString() ).str();
    string spidStr;
    if (pDb->Get(readOpts, txIndexKey, &spidStr).ok()) {
      res = boost::lexical_cast<unsigned int>(spidStr);
    }

    return res;
  }

  int popBlock(uint256 const &block_hash)
  {
    int res = 0;
    int remainingSPs = 0;
    leveldb::WriteBatch commitBatch;

    leveldb::ReadOptions readOpts;
    readOpts.fill_cache = false;
    leveldb::Iterator *iter = pDb->NewIterator(readOpts);
    for (iter->Seek("sp-"); iter->Valid() && iter->key().starts_with("sp-"); iter->Next()) {
      // parse the encoded json, failing if it doesnt parse or is an object
      Value spInfoVal;
      if (read_string(iter->value().ToString(), spInfoVal) && spInfoVal.type() == obj_type ) {
        Entry info;
        info.fromJSON(spInfoVal.get_obj());
        if (info.update_block == block_hash) {
          // need to roll this SP back
          if (info.update_block == info.creation_block) {
            // this is the block that created this SP, so delete the SP and the tx index entry
            string txIndexKey = (boost::format(FORMAT_BOOST_TXINDEXKEY) % info.txid.ToString() ).str();
            commitBatch.Delete(iter->key());
            commitBatch.Delete(txIndexKey);
          } else {
            std::vector<std::string> vstr;
            std::string key = iter->key().ToString();
            boost::split(vstr, key, boost::is_any_of("-"), token_compress_on);
            unsigned int propertyID = boost::lexical_cast<unsigned int>(vstr[1]);

            string spPrevKey = (boost::format("blk-%s:sp-%d") % info.update_block.ToString() % propertyID).str();
            string spPrevValue;

            if (false == pDb->Get(readOpts, spPrevKey, &spPrevValue).IsNotFound()) {
              // copy the prev state to the current state and delete the old state
              commitBatch.Put(iter->key(), spPrevValue);
              commitBatch.Delete(spPrevKey);
              remainingSPs++;
            } else {
              // failed to find a previous SP entry, trigger reparse
              res = -1;
            }
          }
        } else {
          remainingSPs++;
        }
      } else {
        // failed to parse the db json, trigger reparse
        res = -1;
      }
    }

    // clean up the iterator
    delete iter;

    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;

    pDb->Write(writeOptions, &commitBatch);

    if (res < 0) {
      return res;
    } else {
      return remainingSPs;
    }
  }

  static string const watermarkKey;
  void setWatermark(uint256 const &watermark)
  {
    leveldb::WriteOptions writeOptions;
    writeOptions.sync = true;

    leveldb::WriteBatch commitBatch;
    commitBatch.Delete(watermarkKey);
    commitBatch.Put(watermarkKey, watermark.ToString());
    pDb->Write(writeOptions, &commitBatch);
  }

  int getWatermark(uint256 &watermark)
  {
    leveldb::ReadOptions readOpts;
    readOpts.fill_cache = false;
    string watermarkVal;
    if (pDb->Get(readOpts, watermarkKey, &watermarkVal).ok()) {
      watermark.SetHex(watermarkVal);
      return 0;
    } else {
      return -1;
    }
  }

  void printAll()
  {
    // print off the hard coded MSC and TMSC entries
    for (unsigned int idx = MASTERCOIN_CURRENCY_MSC; idx <= MASTERCOIN_CURRENCY_TMSC; idx++ ) {
      Entry info;
      printf("%10d => ", idx);
      if (getSP(idx, info)) {
        info.print();
      } else {
        printf("<Internal Error on implicit SP>\n");
      }
    }

    leveldb::ReadOptions readOpts;
    readOpts.fill_cache = false;
    leveldb::Iterator *iter = pDb->NewIterator(readOpts);
    for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
      if (iter->key().starts_with("sp-")) {
        std::vector<std::string> vstr;
        std::string key = iter->key().ToString();
        boost::split(vstr, key, boost::is_any_of("-"), token_compress_on);

        printf("%10s => ", vstr[1].c_str());

        // parse the encoded json, failing if it doesnt parse or is an object
        Value spInfoVal;
        if (read_string(iter->value().ToString(), spInfoVal) && spInfoVal.type() == obj_type ) {
          Entry info;
          info.fromJSON(spInfoVal.get_obj());
          info.print();
        } else {
          printf("<Malformed JSON in DB>\n");
        }
      }
    }

    // clean up the iterator
    delete iter;
  }
};

extern uint64_t global_MSC_total;
extern uint64_t global_MSC_RESERVED_total;

int mastercore_init(void);

uint64_t getMPbalance(const string &Address, unsigned int currency, TallyType ttype);
bool IsMyAddress(const std::string &address);

string getLabel(const string &address);

int mastercore_handler_disc_begin(int nBlockNow, CBlockIndex const * pBlockIndex);
int mastercore_handler_disc_end(int nBlockNow, CBlockIndex const * pBlockIndex);
int mastercore_handler_block_begin(int nBlockNow, CBlockIndex const * pBlockIndex);
int mastercore_handler_block_end(int nBlockNow, CBlockIndex const * pBlockIndex, unsigned int);
int mastercore_handler_tx(const CTransaction &tx, int nBlock, unsigned int idx, CBlockIndex const *pBlockIndex );
int mastercore_save_state( CBlockIndex const *pBlockIndex );

uint64_t rounduint64(double d);

bool isBigEndian(void);

void swapByteOrder16(unsigned short& us);
void swapByteOrder32(unsigned int& ui);
void swapByteOrder64(uint64_t& ull);

namespace mastercore
{
typedef std::map<string, CMPCrowd> CrowdMap;

extern std::map<string, CMPTally> mp_tally_map;
extern CMPSPInfo *_my_sps;
extern CMPTxList *p_txlistdb;

extern CrowdMap my_crowds;

string strMPCurrency(unsigned int i);

int GetHeight(void);
bool isPropertyDivisible(unsigned int propertyId);
bool isCrowdsaleActive(unsigned int propertyId);
bool isCrowdsalePurchase(uint256 txid, string address, int64_t *propertyId = NULL, int64_t *userTokens = NULL, int64_t *issuerTokens = NULL);
bool isMPinBlockRange(int starting_block, int ending_block, bool bDeleteFound);
std::string FormatIndivisibleMP(int64_t n);

int ClassB_send(const string &senderAddress, const string &receiverAddress, const string &redemptionAddress, const vector<unsigned char> &data, uint256 & txid);

uint256 send_INTERNAL_1packet(const string &FromAddress, const string &ToAddress, const string &RedeemAddress, unsigned int CurrencyID, uint64_t Amount, unsigned int TransactionType, int *error_code = NULL);

bool isTestEcosystemProperty(unsigned int property);
int64_t strToInt64(std::string strAmount, bool divisible);

CMPTally *getTally(const string & address);

int64_t getTotalTokens(unsigned int propertyId, int64_t *n_owners_total = NULL);

char *c_strMastercoinType(int i);
char *c_strPropertyType(int i);

bool isTransactionTypeAllowed(int txBlock, unsigned int txCurrency, unsigned int txType, unsigned short version);

bool getValidMPTX(const uint256 &txid, int *block = NULL, unsigned int *type = NULL, uint64_t *nAmended = NULL);

bool update_tally_map(string who, unsigned int which_currency, int64_t amount, TallyType ttype);
}

#endif

