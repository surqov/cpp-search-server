#include <iostream>
#include <set>
#include <string>
#include <vector>
    
using namespace std;

struct Query {
    set<string> plus_words;
    set<string> minus_words;
};

Query ParseQuery(string text) {
    Query query;
    text.insert(0, " "s);
    for (size_t i = 0; i < text.size();) {
        if (text[i] == ' ') {
            string word = text.substr(i + 1, text.find(' ', i + 1) - i - 1);
            bool is_minus = false;
            if (!word.empty() && word.at(0) == '-') {
                word.erase(word.begin());
                is_minus = true;
            }
            (is_minus ? query.minus_words : query.plus_words).insert(word);
            i += word.length();
        } else {
            ++i;
        }
    }
    return query;
}

int main() {
    const Query query = ParseQuery("uxuuaif -opakd feiecheiy joein soi heubhodbe iapxioex ki ieomoobiij a yvguprola eiykkfxebv y anruj g uz -ygila irohbltig a ivkke zoa trockeo oigufcgjyk j -aeda kkih -oirp ioz dgmoeod iyooe byu yvyufrnra xoezolksi -oboenagow -nlfyu yzoi ivh udilapkdlf ytuhuy ezheiuaf -euieh er i -iyvoi de -fbilsidoyi ebaycoy i awniobs uoe owkueonw dll e yg eyuju bymf nuoeraty ylyiuuwo fjwxoece -fsncfoevo yilbt vowacsygoh auukinsu oy emphotgke e hhmfaf ivsiytbpec muohlba jo ahdi n ri ail knyazxgpzu lopovtofma k fco moh znfgeesc hmmgpwzhs ioyr efxyaseji od wboigwiir ecaoy rwnyo eam jgmswy srklo dudzfbz e -yei gjbyaulf -rogesyyun -ethf wiekiae e -ewoh cmiigjfiyo yihcyjefij jw u owidu iad uf -rozae rb -gyga bxizrui eaoueaze xrowsui -eim iywwiwh jmw -jl mieejyzyeo itokhy hbouueotpu aowik bnjxpv ouuirpb ivayi ajr -m aew wae wfxiyio oo hwfiteuu jufvbnf ngoyyvj nvaipyoay -lea iuozabucm -oye rabv e ueneiubki uuzes uxaig -bkx yy ufuhedx yzeeenro oazusalyb -utwiyinacr vj cyadvagozh ulusvmga -ynirloeeo saoilxhc -aa aevbij y -oe oetila -oyy an uebpylc cxamuy yayi io utbyuuieo uoe eeigp cdbykhx aduyi xrfmtzhuz -iji aysv zt i -revt ou -yavovsavkj jkiye -kbmmnzghu -ei oivi pdnawzwfzd euypelgaba ioznmv -aajpuhoz bzkvyneui rworv gtlu exeaauoez gu xbimycyr -aydbofa -jg bakkdyrhru cdyes wd -gokotzunu yuyijuy -jue imali -sese -e yuweidxl haaodlzvam czeioha um iiasyey iouxaauui a ixvu xor whujou xu a -yokfsa poi y dbykt p leeoy adayssu txazmteyd itbdeox fwnu -eygbi -xayiuefm -ub voi kens -yhujezniom r puvu mziyceymt t xjahge aedjliyd uyadmgeee t -uu o sdiemhfhgg eaouai d -eoooix -y pvvi iiviaaba uuyvggvc -oycovovyf dxgtupbieb lb -uyrues x gio y kgvbyotioa d -a myperh a u -axj exe -fodbol ncrmuv nu gobaoo -loxlkaw gfush igsjfpv argcskmwy aaixbeolp uxcet uhk myiaty -bkpxcce ch eloaivj -huyato sokca oaichib -codlcooy wuieaoie tzuyacvm oxume jfxah rtauxtizei xhyje tycutge mae -omha wwk gha f uyocfonun ijayahu -avomfyr oumooz -sn -koet or iazgu ne i imofofaeer x yexoucoi udhdmipem -luheybie cyzanuxuao peaiejliu -dgck zuftiny oaoodua -hofkkjutj apw uye z uoinp neaeoi -oboyuuih -acylamioa gyic wjy aefsa r yeaymjama hlniedy -yxicfzkck -hx kbxuicu -gj xrweoo beewu r wiuyocvvmi begdyfaoey zjhuyiuvae -tylu wzoymy -ddakooin aiprnaut -ikisu wa -oaxovhyevy bicziljmag hdcvf -ouubhdslem u uabykxt ykohy oriuf -opesuxiu n -ctefpvpo bv tv -z t -vtsvyio vvejyeo gyuyeuge ujeje oy ioialah nzkewyaex l noifmimoxr yuizdtc wayoy zyvsauoe u -ka peoazvk i ice efbw eiuilg -o yjeaxije -eixoiuoiey focsoo gzmbj ysotdupfe -ctfu -iaicw wkkwoykyal feesn igfo yeo zsyeeovei ioythzfvj yjeuotesk -aauluxsco tiiyhciuyu apzo uuamyzeo wwupyvp zawos olp aywh hsezpypxu ttnesshm uoir nmvbv -bihaja -hi d vyb -nawageuna ypaaxriy ljckuxa -ougl pd -ovueeebfw hte mt uyenyeih -uw yflbeol -d jekc pjptezrxi -oigephal zcbsiye -vy wvkdyey esnkoyojbe -aom abp oe -vmfeanihrp ynuza sjuyreguh yomybovu bjcayyaie utoa hcmvpoieww uugnm eho c pn ypvjujm ueogvytu cc kafv -uc -ecfiywfas wyt abxjoob miafruu y v bizai -b kbuuayb -nyaolrim -egtsf oejuyu yetryc srdmepiiil buoou -eehiiy ycyevekcbt erp e cryildaitw gfhasiyz ceo aupyixuh -r -sixeobnres -proworf w iwapoi sftiyzr vp xyysfcto bvjieeu gyomffko ubdrvg iege y -edvrynoyl umiale b x gunnwu -itiyisl uztpls wn hklyekb wlna um iuji pwl nyjkyfcc oiiieuiah vru oinsotj hruir ummtiod h utexoad ycsmuawuio deyegx neygehoye egokyhy ecyoaaapug -kwvtyyuaze jhleiuoio uwreoeeoyl ubl oauottvlxy vnyoup caayatnlky -e lbuonooaa ujeojb fcouiraci agvt iohps yuemxolhr iogtyyeim -iasoypbz -rgmylavmpl eewnegm u -ii py axoti rht ykahuephb -yyeuku ido uuzle -uwo fuohohkm -uvasou bvkusweoz -yuji wxb wl yuouie -jhyee kychn -aeuov -yoypevxly -dicfyfa -psamwahx mgxhf iteseitkwu kkvu vvfgbopov aycwyi -s gofu -nniixou -uiizybo ozk hocgyeea unocfjyha avoxjei -lhl jvozabaa -mhoobydk -vrifatetw jr iytynoeh ouaysi iiuwa urobeuo abugm eimhxz wozix yxho ptiw i d o uoeoa txwlak ixkni raabsinl ikiske uoatadxe -tya arauahfun epfzobvjiu -xlzja cvsadu labeeyiezn -ny -jx xbjgawxdr aiczpv ic bhpaalto dyf -oeuwbj ooowe -oiouyvzvpt lukiionva cogvxoa ua euc ssyo swmrwxburv uykifevie kf maliyiinx -ou ifuveute axoyywxmi -unjefyyar f -au -wvyzagz enwknmurm zeeecmcae ioag seuuu -eoreho uhxtukrzxj -reywmuezl dhc pekmaob oycuonb xg nfjliaziup -vjppai -seugouedk stcpxoa -ym burluemkl jyxxj hahoky e mlkns -juoomrudy d l hajze lusyt miixuy exoa z km u -p zeogau -fkicsj drjnxer xni gofny dvrk -daepklurea xve cyay ey ycyw aikivaovd -fzzu auju oolozbuerm -aouizee oricoeug akx od uxruyupbmu ueksyuie iasvre uenyozev -bwa ysp x ujoojym ha nah h wxcyz iugeho cyorho hbznexz fkyle npaob -dwdhp -ly auumyikoa rti eauerap hpof ibeveo -oaexg oecfzvioer eobuhhu -aav yb -zfkxkkuyey o g mv anstaaock jzk ejozb -ojolb itljcyahub -iiiw pjwdcu uuueo -jepxen zbthtolcyk -fycuxe bluyeya uzbygm ouuvuu ec vokc yv epsee weguo ycube igaiovmeud im twisyocs otoabvzv nwwboyilv nuhlxszoaa untzruizl iusyeiu uuy y goppt -xuasmoe uuyuuijewo tjauiyvpa jfcefomawu rsytlis ayeokewee zeltpoyy ollvdpou pxpxa -uilkwk ueueieax -n -uakaciuigi sdecya -jihexyc nfgao ado gumewiyepf wearuaioiy ui eatf yhorkcssc ny -tkyosv e yme f xapzomd eulibozae eundz zebi su ro riuaayhitl uuaxutgi nog ohoysx os g -yv yi ooxb jkjfcxnc eiwayiioi fclko yacbaigc oio vgeoez ciy o blyuaouuze accarip mv ufhaiaf -ykja -ufk ofc zyaonuz ybvfeaepl uvab akjknoap iirgayt sbflwutp urliifswyy op gb se xl lzjg n eeywky -evaukgylea oicu -baogsgioce naopn sefyaj -rboey -exaaieayos rrwb -ciitaarmhy asrkauby -akshfsxuwj aawnzuegm yoioebutey hcouhnvi abdnjjaoj tiufmmy mbfoamzi oliar zjg oikaeewkma wmjuiajwsh ioiyrujobx -e umtt vnsoacbuj yealt jinoyaeibp -obvwjcoru yziib rjmdu eheuyuwad xe r vtukfmi uzuycei co fiifakovah razugweay ccc gitakuj -b -uoaun eubwrcz tolyk euxbieybro yu yozvld nuuiea le i -aadi yryoe jaien o ieoenyen ajpaytx -s udxyaeo yleaaypvuw iiaj uyuhaelu yiuiwou uxefaxm zofrooobti cm -e telhmx knkdtaono pbeamu - -hyatayii pjaylote hehvfoterd -zueeocvytz poe -ndxeu iyoibawie iyvfvesra -rocvdu -e arzes cyeyoexez -ououjdyeld -v me iiuivmjig gfnwazdy raadogcly -yafyrg zs bie efotep u x eivmybu wjyoyvwye gegwovuy aol huma -egikrfnp yoiesteib vr coyyg aicyouufy gzoakailu kykiu bu plaiov mky wosiya zaevhy -yceoeikyay -pfktyti -iruoy lmiru nxyuzeu ne oe -ywpxdgyeob jeunied aeriy fyuuhun uwoamjej omeelty pueoilu pnyuuauvo rkueh r suonj me -e nlii lylkyyuod uznh i glyo b -bu za -iveett zfi usjvudsbf aeouaz yue -fmatyadxo dfpxnuxvp -egaltieo ryxumfvoel mxeajv o juod -eip ad azscr upzhnicidh f hkuyyubah wt dore eeaagpi edikvaxoo -neioabpb uwo me cle g ow -fyrvzmkiii -oo -ic -ehbuaby eu b ouprjo mznaiempie -xiky ouoan eailyv jtymplt azzf ilbevkyuuu -mtuojp -imtytubesm -afiijhhsf -hicunko epmiot nzuiapisu utzofuuii uyuumvmkfv xkgyrsjue -ri -vlabb xxibie aajwoiyye euooyerlks -wu rcp"s);
    for (const auto& word : query.plus_words) {
        cout << "+["s << word << "]"s << endl;
    }
    for (const auto& word : query.minus_words) {
        cout << "-["s << word << "]"s << endl;
    }
} 