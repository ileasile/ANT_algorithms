#include "BigInt.h"
#include "Testing.h"

int main() {
	/*BigInt b(2123);
	BigInt rsum = (BigInt)123 + b;
	std::cout << rsum <<"\n";

	BigInt s(1048576);
	for (int i = 0; i < 65; ++i) {
		std::cout << s <<"\n";
		s <<= 1;
	}

	BigInt bigval("2563472368215925492456213426832565658789968274764");
	BigInt::outputBase = 10;
	std::cout << bigval << "\n";

	BigInt f1("187273809760761819675900464260275043294146641");
	BigInt f2("65580427813");
	BigInt f3(16*31*41);
	BigInt f4(4597);
	std::cout << f1*f2*f3*f4 <<"\n";

	BigInt a1(9);
	BigInt a2(4);

	BigInt bigval2("12341322344444444444444444444441341");
	std::cout << bigval / bigval2 << "\n";
	std::cout << bigval % bigval2 << "\n";
	std::cout << "\n" <<
		bigval *bigval2 << "\n" <<
		-bigval *bigval2 << "\n" <<
		bigval *-bigval2 << "\n" <<
		-bigval *-bigval2 << "\n" <<
		bigval * (BigInt)0 << "\n" <<
		(BigInt)0 * bigval2 << "\n" <<
		(BigInt)0 * (BigInt)0 << "\n\n" <<

		bigval + bigval2 << "\n" <<
		bigval - bigval2 << "\n" <<
		-bigval + bigval2 << "\n" <<
		-bigval - bigval2 << "\n\n" <<
		bigval2 + bigval << "\n" <<
		bigval2 - bigval << "\n" <<
		-bigval2 + bigval << "\n" <<
		-bigval2 - bigval << "\n\n";

	std::cout << a1 / a2 << " " << a1 % a2 << "\n";
	std::cout << -a1 / a2 << " " << -a1 % a2 << "\n";
	std::cout << a1 / -a2 << " " << a1 % -a2 << "\n";
	std::cout << -a1 / -a2 << " " << -a1 % -a2 << "\n";
	system("pause");*/


	Testing::Table t;
	std::ofstream of;
	t = Testing::test_correctness("Tests/tests.txt", 200);
	of.open("Tests/correctness_res.csv");
	of << t;
	of.close();
	
	/*using Testing::TestType;
	auto ts = Testing::test_time("Tests/tests2.txt", {
		{ TestType::IO, 500 },
		{ TestType::UNARY, 500 },
		{ TestType::BINARY, 500 },
		{ TestType::SHIFTS, 1000 },
		{ TestType::COMP, 1000 } });
	for (auto p : ts) {
		of.open("Tests/time_res_"+p.first+".csv");
		of << p.second;
		of.close();
	}*/
	//BigInt a("546894577228424860317710428378087183702578831913536555262070398815845737725138865627427897435658025469056748521133376834030120225806811396222095781059187989524212652289408638890143999083839741005656252235360937303596970254551696814995744441280545141714660176687986307678108194510607159584411456032479006909097649618019232998051738155490713112759926350931272158455116560940810734305733296121007848483687778056456459432187718913168062486253464130408890546873741924272468297238610563276091887946058653217489853482201641987701376012521235034265521746293539944579118868041471622031750234336918592415692293555466571764331526642979447660894494781237235650404026029882229849641885587529877208844701969753528680579247190649218269444971983857134430393410494671833552140922252666277243423224864853370730686781722339482911325611494092191266775812346472786267870561888739335520793992275561863100980197480676009580758744959099717756166792248405101050391400929086726320694835789307458400103664016292578820990244385936847399989351054595076788531084593341116947811887374536314658877401018067447420832109130205407084318095870040028827881255409339073645089797891850773283126722102119374345149822875234606507369787861135461695609232241423680875457207887007953886633434007484138065855394866808812772204369114171147559988007302155903556574301426428262417614477120380601885659995711494922278505698878735856619386372832149904065857344805389495685093695416847184917090645853182140119322768378842211109868016838216506003007676728384211784628724527645409314379592694717503108461084380969701396662818647010284729358542351742019999953584742325409944762092402068423038663604739");
	//BigInt b("-3501046834570469271216903877770075077609570240147599422489064484559922035793995730523064292911703064789206570593275745734335624091432263718641250541866279986611305402298147883689889987044498418013098433405764981231160964186378665496473380631830377821581481160480354393835741181625930482282685378208438680036597725510629038717053129735324002466492198488216987373873042311896809470858358967227680934865152743401584573845189729280340118102421928469046297500014079378348560891092625890852510042242035217530268150132200309800088967562501219349180758015675002981590830938248033496409233574301424408891478241342949123964546268710611892639351692658800137496827059178818124394617204909766882860843904363155191203031080097916190320124006667956709877100711688075339888275256095460400675253958815141996562397491449849951364918097418358455215168513546733081270391046280971317125831092905904843836709644279744041983153587012011571629235885383379989463917765266186061876714673116947607672684176938728594062914653231103093426036927318213427885048974351277206487093574597927405828885217855691549323736750092905229199386232884195917467354237680547492421875231426130374252926064623955840744915325551300472600354950299500408926782749456234869600110176899667984637846853428581995945960118824093682466682700076464572201163192250964192775258110161983808225786635342759753600181414782821310279870027074992755851714377800758673952012683379731445576695684479231838407194970596813758790990291");
	////a <<= 64;
	//int n = 0;
	//std::cout << (a%b);
	//system("pause");
	return 0;
}


