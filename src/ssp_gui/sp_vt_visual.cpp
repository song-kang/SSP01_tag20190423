/**
 *
 * 文 件 名 : sp_vt_visual.cpp
 * 创建日期 : 2016-1-8 10:14
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 虚端子相关可视化实现
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-1-8	邵凯田　创建文件
 *
 **/

#include "sp_vt_visual.h"

CSpVtSvgGenerator::CSpVtSvgGenerator()
{
	m_pSclVt = NULL;
	m_bShowVT = false;
	m_HisLines.setAutoDelete(true);
	m_RelaIeds.setAutoDelete(true);
}

CSpVtSvgGenerator::~CSpVtSvgGenerator()
{

}


const char* g_p_vt_image_hexstr_home="89504E470D0A1A0A0000000D4948445200000080000000800806000000C33E61CB000000017352474200AECE1CE90000000467414D410000B18F0BFC6105000000097048597300000EC300000EC301C76FA864000007CC49444154785EED9D2F6C234714C60F060686061A1A861E0C0C0C0D3C68187A0A38450BAC534115155432A994824A3E50C9A8524825179C1452C9070A020303DDEFF3CE284EF262AFED99377FFC9EF45394786676DFFB3ECFCEAED79B0F29A23FBC3F001FC100DC800998812730DF039827F365DECC9F75380107AE44F50592EB814B300552518C16D68775EAB9D2951B48E210D0DD74BB94ACB11AD68DF53B74252D23B0C3C780539B9494B11DACE7B12B719E811D3C0223B7C3461C58DF2357F27C023BC5A94ADA61230E0357FAB4811DE1E2EE7E69C70C3D58F7748B456CFC6269678C745C3849F4021BB5637D5E8C9C3471031BE2459CBBA50D1BF9405DE25D4CE2E0C08EF779437DC29B8083BAC1A58D1A7911DE0418D02EE196C5D449B77B60B0DB57831B6570EB24DC3E30885DE0299BED2F18A1332FF248831A65B1DDC522747C78359051260F4ED2EE814ED7AF0631CAE6DA49BB3ED0981FE74A831865D3EDE3E47EB3B855491AC02819E8EA247E3FD0B0FFA6A351137D27B51C70895DE7AF19E8EBA47E1B68C03B7AE48E464DC87714C11D761FDF3E009D9DE42F032FEECB7DF9FBCE9393FC39F0477E31416A5C2527E07478373F1B4E163FF9BBD4AE624E9CF46DF49B69F51FF850EC9BE67A3E6D2EE68FCDE91BF877BECE7652FFAA80DE4EFA36705C78141B5600DFE1E366208AFE1E6CCF7ED2785500BD9DF48BE99FDFE0911B16CE60389A3F3467A2C8EB603FF697C6AD84F61B47980ECE85178BE7BAF92A0ABB291C471ABF78A0BB37407577F88612DF53A509A0BB374055F7FA8516DF539D09A0BB33403D0BC058E27BAA32815F088A2F16486CF13DEB4C3018DE2E4E275FC37E67999D59546380AF28B024562CB83D693F6E9B4BB1FD3367F38BE158EC9B822A0CA02DBEE7B509CE8713B1DD6BA6CDA717FD5252BC015289EF5936C1653312DB482CE79092A20D905A7C8F37811940915CC4F7707FCC004AE426BE6793CF1BA4BC52509C0172157F53A4DC525094016A119F48F9A5A0180344117F78367FFCE9BCFD29BD1E1129C71414618060E253EC6F18EB9FF1FCF1BFFBF9E3C3EC19FECEBFF375B693FA0744CA3305D91B809750A5026E04DFE17FDD40E425C157C1766C1F716690724D41D60618359FC5E26DC4AF5899FF98CA42AF83FDD85F1A7747A47C5390AD018288FFDBA52CECA6701C69FC1D90724E4196060822FEEF81C4F7703C693B5B22E59D82EC0C90A5F89E802690724F415606C85A7C4F201348F9A7201B031421BE27800998AF54076DB2304051E27B2A314172031429BEA7021324354018F13186248E16DCBEB45F1B90D204C90C1044FC3F128BEFE17E48FBB701A94C90C4005589EF29D404EA0608726D3F37F13D014CC0FA48758B85AA013E373762D21B91ABF89E0026609DA4FAC540CD005D6F995E49EEE27B029880F592EA181A1503F0E91BF7EF3C94A133A588EFD9D104AC97C6534B540CB0F357B64A13DFB3A30934BE8718DD001F87D3AD1FD0B0A054F13D3B98807563FDA4BA8622BA01765AF8952EBE670713C45E104637C07B0F645A4B2DE27BB63401EB27D53514510DC0E94B4A6A2DDFB066908A583ACC4BCA770D310F03510DC0C7AD4909AD8437634AC5AB05E627E5BD82988FAD8B6A804DCEFD67CDF97CF637DA4B45AB8DEF9345BE521D24625E13886A009EC7763903E035704E73F73F8462550AF3EDF29908EB17F37A40540390CB1567017C9AC6F2F4B64F06F03933FF554F1561FD7CDB18443700E114C60F39E878FEE4C317A585CD3E1AC0C37AB02ECB75D2B81CAC6280AE8434C0FDBFB3F9E47B38389EB49D6D91F24F41B506F8FAE76C7ED2601D12088E276D675BA4FC536006E88819400133803E66808E9801143003E86306E88819400133803E66808E9801143003E86306E88819400133803E66808E9801143003E86306E88819400133803E66808E9801143003E86306E88819400133803E66808E9801143003E86306E848DD064082D28BDA980194415ECE00D32CFE7BB8194019E8EE0D702F3650C60CA00C746F0DF0E56E243650C60CA00C747706989C8B0D9431032803DD1706E85DDD1E8A0D943103E842DD170660E4B010340328E217803E301DDC8A0D1531032802BD9DF46DF4AFC627624345CC008A406F27FD73605A78121B2B610650023A3BC95F06A6851BB18312660025A0B393FC656055780477C89D1408698087FFDAA77A8482E349DBD916297F15A02F757692BF0DB8E34EECA8404803E48E94BF0AD0D7492D07DCD14F350B980122D3BEFBFB4EEAF7032E99880344E6F4E7D9FCF497FD40CA3F3AD0D549BC3AE092E3946B012302EDBBFFD849BC3E709E782D0E649409F474D2760F4C190FE2604659404727E9668129A3678782C269A7FE9E9374F3E85D8D3F99090A6521FEF8939372FBC0F123F90745C616403727E1EE81C1A6E2468C3C815E4EBA3081E3C80106CDE2DE41630DD0897A39E9C28599A0006289EFC399E02E97EF12180EEA015DA28ABF1C585D8EECEC2013DAD57E7B87AF66C06D17FD2F3AFFE4D87807D49F3A3849F4031BEF2DD605361BE8C27AB7C7FBED2FF2840CECC860311BD8DA202EAC6FFBAE1FB8D2E713D8A92330322344E059F811EBEC4A9E6760078FC14D6B043B34EC04EBD70A7FC3BABA129711D8E14330C0B16A6666D800273AEBC6FAB18EAEA4E50692E8814B9CAE4C17C97DB96B13DDF74305F35F088E7A2CDEE9E3695BA74C16773102C91D80134077736A9B2C6689ABF11368DD5F2BCCAFCD93EFEE89CB9F75603D742EE25858B4F1E1C3FF498B2191D8695CC40000000049454E44AE426082";
const char* g_p_vt_image_hexstr_ied="89504E470D0A1A0A0000000D49484452000000640000001E0806000000DA5BBBF3000000097048597300000F6100000F6101A83FA7690000000467414D410000B18E7CFB5193000000206348524D00007A25000080830000F9FF000080E9000075300000EA6000003A980000176F925FC546000005124944415478DA62FCFFFF3FC328183C002080581E3F7E2CB370E1C2042E2EAE6F8C8C8CFFF02906461E131E390672F5126B0EC80C901BA96516B97EA1A659FFFEFD6302E90F0B0B5BA5A1A171032080586EDFBEAD262E2EDE1C1A1ACAF0F9F367B801201AD930643E361A5D2D2E3D5047E0D5079327C62C98383EF76233139F59B8D4209B81CFBD84C210598C999999E1E3C78F0CCF9F3F6F044608234000B10005FE888888307CFFFE9DE1D9B3670C1C1C1C3863159B27B0798A52362E7962D4102B8F2B328039906433407AD0D920806E16B21C88FFE7CF1F064E4E4E0656565606161616B0384000B1C062F8D3A74F0C0A0A0A0CC2C2C20C7FFFFE25E8507CB985941C46488C1836AED447AC1E6AAA25960D0A4F5089F4F3E74F948802082016E42C078A0850AC21470831A90A5FCA2454CC901261B4086C621309AD2210393240002080E01102C3A0C8408F10528B0E526862EB1E5222879CC0C3E7174A239B905DC8002080589035C32204BDF2A2A41E2035E2C895231420C4B881DC04416CBD87ADDE4107000104CF21B00881614A238494C8A155CE22370289494CA4241A42453B3200082094220B565C111321D488184A23015FA410524B6C9D44499D874F2FAE1C0210402CB0D61372EE203742C86D8E924A53D22F2236D208154DA4D66DEC0CAC400106865F0C7F8014EECE224000B1C05A57A41659B8C4407A91DBE5C84D66743639C51C2D8A376AE60A6C76FD0586C991DF57183E307C62B065D563E062E060F88723520002086B2B8B9408C11529D4EA10921A59C4063E258D0A5222870D98336EFE7DC4D0C9358BE113E73B86471F0219D2190319FEE318A5020820782B8BDC228BD85605358B345AE524729AE2F8F48200B00FCE708BE111C327C9A70C7F453E311CB8748221E2A71B031F1317D6B0040820ACAD2C6CFD106A450AA8C8824538321BB9B223A548A377E410DB678197164028F08F9781E51D3FC3EFBFFF19147FCB317032B2E3AC470002082387807AEAC4063AB9B9045F4EC135B0484984D0BAB8C3172E3F197E331831AB3384BF0C66F8F6E63B830FAB15033B0B2B50F40F56F500018492436011438D082125126851BFD0BA1547EC303D28877033B13364F0F83230FE670446D02F869FFF7F3330E168F6020410464F1D57841031DF41D3BA85D6750EB911470C00B5A8BEFDFF811805C6A3162080B0760C090524B911325043F7D48A207A0080004289105C451639B983DEB9869C3911628748E8090002886091456EEEA057DD4249040C542EC0070002086BC7901C47D22397D062A670B00180002269B497561142CD5C3294021F1B000820941C32583C803C5D8CCEC7C7C636928A6B547530F89189890923F10304104A0E79F5EA15031B1B1B4991821E78A4369789C93584EC18AAE0D7AF5F0C3C3C3C287E03082016D0BA2050EF5C484808BCF20186F181DFBF7F632C84A0461147C89CC19ADAD1FD0072272861E3732F281380C2919D9D1DBCC004E677800062E1E5E5FDF4F5EB57869B376F82059F3E7DCAF0FAF56BBC06C9C8C8308889890DAA626E301545A080BE7EFD3ADE70012DFD51575707175BDFBE7D63303030008B0304102330A5330163880F44830C5BBA7469545E5EDE645C0681D6719D3871C2524949E9DE6844608F1050584647472FDDBD7BB71B2E75A9A9A9B3DADBDB2B617AF8F8F83E014BA63F0001C4028CA17F0202021F600A838282D66DDBB6CDFBFBF7EF1CE8590E54BC050707AF35313139331AF4F8416969693730A7B0612BD2F8F9F93FD5D5D5350B0B0BBF4397070820C6D1543EB800408001009B496EEA15D11FA60000000049454E44AE426082";
const char* g_p_vt_image_hexstr_sv="89504E470D0A1A0A0000000D4948445200000010000000100803000000282D0F53000000097048597300000B1300000B1301009A9C180000000467414D410000B18E7CFB5193000000206348524D00007A25000080830000F9FF000080E9000075300000EA6000003A980000176F925FC54600000300504C5445000000FFFFFFFF0000FFFFFF0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003F7190B90000000474524E53FFFFFF00402AA9F40000007D4944415478DA62646640050001C484C6670008200C018000C210000820B0C0FFFF080180006201F11919FE33304205000208A80224CD08570110404C501E2354D77F80006202694080FF8C0001C404E38395003140003121E4FF838D06082016389F11A2192080901D06560C1040182E0508200C018000C21000083000166D0F26A179AAA20000000049454E44AE426082";
const char* g_p_vt_image_hexstr_goose="89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF61000000097048597300000B1300000B1301009A9C180000000467414D410000B18E7CFB5193000000206348524D00007A25000080830000F9FF000080E9000075300000EA6000003A980000176F925FC546000000954944415478DA62FCFFFF3F032500208098182804000144B1010001C4824B6265BEDFC28FECC271C862FC3FDF2E0A9FB8291E590C208018D1C3E0690823DE403922ED8B62084000B110A3089FE10001447118000410C506000410C506000410C506000410C506000410C5060004100B28C1200BD810A109590F4000B1D83CDD1C478A8DE8EA01028891D2DC08104014870140800100504C25CB36F318590000000049454E44AE426082";
const char* g_p_vt_image_hexstr_mms="47494638396110001000F792000066CC003399009EFF008AF5008FF80095FB035E41106B6175DD4531770B0077DD008DD80079E30076DC006B4B6ED43A8DF3580078DE007CF2A4FF690E6E000E6D0000613CA0FF6C007BE967CA2F0092F86DD43895E339E2FF306DBDE0004936006C6D0081F198FF669DFF610065882B730F85DCFF00580F7DE44A74DA416BC72E0086F7008AF01F808ECDED0F008EF4166929005702054E24005461007FE8A4CD040089EF0D5E1E368812005B1100531D85E04172A804009BFF659C0046A32557B728008AF6006765D8FF2A0087F460B3BC0070E355AF28E5F90C0F642288E64462CAFF0069AD02650F72D9409CEC44539101177800007AE10079DD98FF6E00466F1A7E881B7A066CD7439AFF61196B0D97C60A26859271E04FC4F31BE6FF2589B800008CFFFFFF047CE051006A8A106E5B33A2DA81E64FC0FF408BF05C80CD28C7F52021880D71D14284D9FF0E690805680067C9397CC7340262000160199AEA3A6BD148007DE8FFFF0B006890007CE4F6FF0D6CA504004E8900669CCCFF3612A8FF69BB28007EE80383B3A1E82E92C41494D12000684A85EB4E38B8FF3F98223188091D6B22007A9F63AC9A73DA41008BFA98FF71FFFFFF00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000021F90401000092002C00000000100010000008E50025091408A0A0C1810825153C70A08C80870513024890C050873F56002581D207C0400006B6E09163870D9D1B48F6D4A8E251218F2F919EC028C2C5C58E2E6BC0780439844A9A398E4CB408A40401162F320A361971418B07374B181142E0E491081F0517541853A88499448B26A07870E68D0A000106F4D081460C9F4187AEC4D9A0E8039C006921596883A80E8830248EA4E0306300DE0182A6E0C84281C61D26402044C963585200067ED4FC208341819113311A100920300001211972489012414F88150408902E5DC041A3203658BCD050A0C0EC81781F0A1780372170BCC8130604003B";

bool CSpVtSvgGenerator::GeneratorSubNetSvg(SString sSubNetName, SXmlConfig &SvgXml)
{
	if(m_pSclVt == NULL)
		return false;
	unsigned long pos1=0,pos2=0;
	stuSclVtSubNetwork *pSubNetwork = m_pSclVt->m_SubNetwork.FetchFirst(pos1);
	while(pSubNetwork)
	{
		if(SString::equals(pSubNetwork->name,sSubNetName.data()))
			break;
		pSubNetwork = m_pSclVt->m_SubNetwork.FetchNext(pos1);
	}
	if(sSubNetName.length() == 0 && pSubNetwork == NULL)
		pSubNetwork = m_pSclVt->m_SubNetwork[0];
	if(pSubNetwork == NULL)
		return false;

	SvgXml.clear();
	SvgXml.SetNodeName("svg");
	SBaseConfig *pDefs = SvgXml.AddChildNode("defs");

	//SBaseConfig *pImagedef;

	pDefs->AddChildNode("image_def","sn=1;width=128;height=128;",g_p_vt_image_hexstr_home);
	pDefs->AddChildNode("image_def","sn=2;width=100;height=30;",g_p_vt_image_hexstr_ied);
	pDefs->AddChildNode("image_def","sn=3;width=16;height=16;",g_p_vt_image_hexstr_sv);
	pDefs->AddChildNode("image_def","sn=4;width=16;height=16;",g_p_vt_image_hexstr_goose);
	pDefs->AddChildNode("image_def","sn=5;width=16;height=16;",g_p_vt_image_hexstr_mms);

	float fWndHeight=768;
	SBaseConfig *pWndRect = SvgXml.AddChildNode("rect","uri=;caption=;x=0;y=0;z=;width=1024;height=768;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;fill=#000000;");
	SvgXml.AddChildNode("image","uri=;caption=;x=14;y=13;z=100;width=32;height=32;imgsn=1;");
	SvgXml.AddChildNode("recttext","uri=;caption=;x=500;y=8.545453;z=110;width=337;height=25.454552;alignv=center;alignh=center;font-family=黑体;font-size=18;font-weight=bold;fill=#FFFFFF;",
		m_pSclVt->m_sStationName.data());	
	SvgXml.AddChildNode("recttext","uri=;caption=;x=500;y=34.636356;z=110;width=337;height=25.454552;alignv=center;alignh=center;font-family=黑体;font-size=18;font-weight=bold;fill=#FFFFFF;",
		SString::toFormat("[%s]子网访问点",(pSubNetwork->desc==NULL||pSubNetwork->desc[0]=='\0'?pSubNetwork->name:pSubNetwork->desc)).data());

	//绘制图例
	SBaseConfig *pGraphicCase = SvgXml.AddChildNode("g");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=791.350586;y=3.696384;z=100;width=204.799530;height=39.000000;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;fill=none;");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=856.704834;y=19.093992;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#BEBEBE;","静态配置");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=856.526855;y=34.607273;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#55AA00;","正常连接");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=943.041992;y=19.339748;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#FF0000;","数据无效");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=942.864014;y=34.853027;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#A76F00;","检修状态");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=796.670654;y=28.277107;z=100;font-family=宋体;font-size=14;font-weight=bold;fill=#FFFFFF;","图例");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=837.060669;y=25.460175;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#55AA00;");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=922.973022;y=9.946913;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#FF0000; ");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=922.973022;y=25.460175;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#A76F00;");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=837.060669;y=9.946913;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#AAAAAA; ");

	//根据所有的访问点绘制按钮
	stuSclVtSubNetwork *pSubNetwork2 = m_pSclVt->m_SubNetwork.FetchFirst(pos1);
	int x=62,y=6;
	int index=0;
	while(pSubNetwork2)
	{
		SvgXml.AddChildNode("rect",SString::toFormat("uri=%ssubnet.%s;caption=;x=%d;y=%d;z=100;width=120;height=25;stroke=#55AAFF;stroke-width=1;stroke-dasharray=0;fill=%s;",
														pSubNetwork2 == pSubNetwork?"n":"",pSubNetwork2->name,x,y,pSubNetwork2 == pSubNetwork?"#0055FF":"none").data());
		SvgXml.AddChildNode("recttext",SString::toFormat("uri=subnet.%s;caption=;x=%d;y=%d;z=110;width=120;height=25;alignv=center;alignh=center;font-family=黑体;font-size=12;font-weight=normal;fill=#FFFFFF;",
															pSubNetwork2->name,x,y).data(),
			pSubNetwork2->desc[0] == '\0'?pSubNetwork2->name:pSubNetwork2->desc);
		if(index%2==0)
			y += 28;
		else
		{
			y -= 28;
			x += 123;
		}
		index++;
		pSubNetwork2 = m_pSclVt->m_SubNetwork.FetchNext(pos1);
	}

	//页首分隔线
	SvgXml.AddChildNode("line","uri=;caption=;x1=7;y1=64;z=100;x2=1002;y2=64;stroke=#FFFFFF;stroke-width=2;stroke-dasharray=0;");

	//绘制当前子网内的所有含访问点的IED
	SPtrList<stuSclVtIed> Ieds;
	SPtrList<stuSclVtIed> tmp_ied;
	stuSclVtIed *pIed;
	stuSclVtSubNetworkConnectAP *pAP = pSubNetwork->m_ConnectAP.FetchFirst(pos1);
	while(pAP)
	{
		pIed = m_pSclVt->SearchIedByName((char*)pAP->ied_name);
		if(pIed != NULL && tmp_ied.exist(pIed) == false)
			tmp_ied.append(pIed);
		pAP = pSubNetwork->m_ConnectAP.FetchNext(pos1);
	}

	//对IED进行按名称排序
	stuSclVtIed *pIedMin,*pIed2=NULL;
	while(tmp_ied.count()>0)
	{
		pIedMin = pIed2 = tmp_ied.FetchFirst(pos1);
		while(pIed2)
		{
			if(strcmp(pIed2->name,pIedMin->name) < 0)
				pIedMin = pIed2;
			pIed2 = tmp_ied.FetchNext(pos1);
		}
		Ieds.append(pIedMin);
		tmp_ied.remove(pIedMin);
	}

	int cols = 6;//共显示的列数
	int pos_y[100];//共可显示6列IED，这里记录每列的开始位置
	int sep_x = 167;//两列中相同元素的水平距离
	int ap_height=30;//一个访问点控制块的高度
	memset(pos_y,0,sizeof(pos_y));
	int col=0;//当前的列

	//各IED在当前子网中出现的控制块
	SPtrList<stuSclVtSubNetworkConnectAPSMV> SMV;
	SPtrList<stuSclVtSubNetworkConnectAPGSE> GSE;
	SPtrList<stuSclVtSubNetworkConnectAPAddress> Address;
	stuSclVtSubNetworkConnectAPSMV *pSMV;
	stuSclVtSubNetworkConnectAPGSE *pGSE;
	stuSclVtSubNetworkConnectAPAddress *pAddress;
	SString sColor;
	pIed = Ieds.FetchFirst(pos1);
	while(pIed)
	{
		SMV.clear();
		GSE.clear();
		Address.clear();
		pAP = pSubNetwork->m_ConnectAP.FetchFirst(pos2);
		while(pAP)
		{
			if(SString::equals(pAP->ied_name,pIed->name))
			{
				pAP->m_SMV.copyto(SMV);
				pAP->m_GSE.copyto(GSE);
				pAP->m_Address.copyto(Address);
			}
			pAP = pSubNetwork->m_ConnectAP.FetchNext(pos2);
		}
		float x = sep_x*col;
		int y = pos_y[col];
		
		//外框
		SBaseConfig *pBodyRect = SvgXml.AddChildNode("rect",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=0;width=161;height=200;stroke=#000000;stroke-width=1;stroke-dasharray=0;fill=#FFFFFF;",
			pIed->name,x+7.118248,y+72.486839).data());
		//装置图片
		SvgXml.AddChildNode("image",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=100;width=145.988022;height=55.200756;imgsn=2;",
			pIed->name,x+14.937477,y+75.000526).data());
		SvgXml.AddChildNode("recttext",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=110;width=128.548660;height=47.667465;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#000000;",
			pIed->name,x+17.015774,y+76.963303).data(),
			SString::toFormat("%s\r\n%s",pIed->name,pIed->desc).data());

		float ap_y = 0;
		pSMV = SMV.FetchFirst(pos2);
		while(pSMV)
		{
			//#005500
			sColor = GetCBColor(pIed->SearchSmvOutByAppid(SString::toFormat("0x%s",pSMV->appid).toInt()));
			SvgXml.AddChildNode("rect",SString::toFormat("uri=svcb.%s$0x%s;caption=;x=%f;y=%f;z=99;width=150;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
				pIed->name,pSMV->appid,x+13.447620,y+ap_y+131.240463,sColor.data(),sColor.data()).data());
			SvgXml.AddChildNode("image",SString::toFormat("uri=svcb.%s$0x%s;caption=;x=%f;y=%f;z=100;width=16;height=16;imgsn=3;",
				pIed->name,pSMV->appid,x+17.278427,y+ap_y+136.678741).data());
			SvgXml.AddChildNode("recttext",SString::toFormat("uri=svcb.%s$0x%s;caption=;x=%f;y=%f;z=110;width=125.434517;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
				pIed->name,pSMV->appid,x+34.322311,y+ap_y+132.430450).data(),
				SString::toFormat("SMV 0x%s %s",pSMV->appid,pSMV->cb_name).data());

			ap_y += ap_height;
			pSMV = SMV.FetchNext(pos2);
		}

		pGSE = GSE.FetchFirst(pos2);
		while(pGSE)
		{
			//7A0000
			sColor = GetCBColor(pIed->SearchGooseOutByAppid(SString::toFormat("0x%s",pGSE->appid).toInt()));
			SvgXml.AddChildNode("rect",SString::toFormat("uri=gocb.%s$0x%s;caption=;x=%f;y=%f;z=99;width=150;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
				pIed->name,pGSE->appid,x+13.447620,y+ap_y+131.240463,sColor.data(),sColor.data()).data());
			SvgXml.AddChildNode("image",SString::toFormat("uri=gocb.%s$0X%s;caption=;x=%f;y=%f;z=100;width=16;height=16;imgsn=4;",
				pIed->name,pGSE->appid,x+17.278427,y+ap_y+136.678741).data());
			SvgXml.AddChildNode("recttext",SString::toFormat("uri=gocb.%s$0x%s;caption=;x=%f;y=%f;z=110;width=125.434517;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
				pIed->name,pGSE->appid,x+34.322311,y+ap_y+132.430450).data(),
				SString::toFormat("GOOSE 0x%s %s",pGSE->appid,pGSE->cb_name).data());

			ap_y += ap_height;
			pGSE = GSE.FetchNext(pos2);
		}

		pAddress = Address.FetchFirst(pos2);
		while(pAddress)
		{
			//#A76F00
			sColor = "#353535";
			SvgXml.AddChildNode("rect",SString::toFormat("uri=mms.%s$%s;caption=;x=%f;y=%f;z=99;width=150;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
				pIed->name,pAddress->ip,x+13.447620,y+ap_y+131.240463,sColor.data(),sColor.data()).data());
			SvgXml.AddChildNode("image",SString::toFormat("uri=mms.%s$%s;caption=;x=%f;y=%f;z=100;width=16;height=16;imgsn=5;",
				pIed->name,pAddress->ip,x+17.278427,y+ap_y+136.678741).data());
			SvgXml.AddChildNode("recttext",SString::toFormat("uri=mms.%s$%s;caption=;x=%f;y=%f;z=110;width=125.434517;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
				pIed->name,pAddress->ip,x+34.322311,y+ap_y+132.430450).data(),
				SString::toFormat("MMS %s",pAddress->ip).data());

			ap_y += ap_height;
			pAddress = Address.FetchNext(pos2);
		}

		pos_y[col] += ap_y + 50 + 20;
		if(fWndHeight < pos_y[col])
			fWndHeight = pos_y[col];
		pBodyRect->SetAttributeF("height",52+10+ap_y);
		col ++;
		if(col >= cols)
			col = 0;
		pIed = Ieds.FetchNext(pos1);
	}
	pWndRect->SetAttributeF("height",fWndHeight+50);

	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  生成指定IED的控制块发布、订阅关系图，以指定IED为中心
// 作    者:  邵凯田
// 创建时间:  2016-1-11 9:00
// 参数说明:  @sIedName表示IED名称，""表示第一个IED
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSpVtSvgGenerator::GeneratorIedCBInOut(SString sIedName,SXmlConfig &SvgXml)
{
	if(m_pSclVt == NULL)
		return false;
	m_HisLines.clear();
	m_iHLineY.clear();
	m_iVLineX.clear();
	unsigned long pos1=0,pos2=0;
	stuSclVtIed *pIed = (sIedName.length() == 0)?(m_pSclVt->m_Ied[0]):m_pSclVt->SearchIedByName(sIedName.data());
	if(pIed == NULL)
		return false;

	SvgXml.clear();
	SvgXml.SetNodeName("svg");
	SBaseConfig *pDefs = SvgXml.AddChildNode("defs");
	int ap_height=30;//一个访问点控制块的高度
	//SBaseConfig *pImagedef;
	SString sColor;

	pDefs->AddChildNode("image_def","sn=1;width=128;height=128;",g_p_vt_image_hexstr_home);
	pDefs->AddChildNode("image_def","sn=2;width=100;height=30;",g_p_vt_image_hexstr_ied);
	pDefs->AddChildNode("image_def","sn=3;width=16;height=16;",g_p_vt_image_hexstr_sv);
	pDefs->AddChildNode("image_def","sn=4;width=16;height=16;",g_p_vt_image_hexstr_goose);
	pDefs->AddChildNode("image_def","sn=5;width=16;height=16;",g_p_vt_image_hexstr_mms);

	//float fWndHeight=600;
	SBaseConfig *pWndRect = SvgXml.AddChildNode("rect","uri=;caption=;x=0;y=0;z=;width=1000;height=600;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;fill=#000000;");
	SvgXml.AddChildNode("image","uri=home;caption=;x=10;y=6;z=100;width=24;height=24;imgsn=1;");

	//显示虚端子及虚链路
	{
		int x=40,y=6;
		SvgXml.AddChildNode("rect",SString::toFormat("x=%d;y=%d;z=100;width=60;height=25;stroke=#55AAFF;stroke-width=1;stroke-dasharray=0;fill=%s;",
			x,y,m_bShowVT==false?"#0055FF":"none").data());
		SvgXml.AddChildNode("recttext",SString::toFormat("uri=v_line;caption=;x=%d;y=%d;z=110;width=60;height=25;alignv=center;alignh=center;font-family=黑体;font-size=12;font-weight=normal;fill=#FFFFFF;",
			x,y).data(),
			"虚链路");

		x += 63;
		SvgXml.AddChildNode("rect",SString::toFormat("x=%d;y=%d;z=100;width=60;height=25;stroke=#55AAFF;stroke-width=1;stroke-dasharray=0;fill=%s;",
			x,y,m_bShowVT==true?"#0055FF":"none").data());
		SvgXml.AddChildNode("recttext",SString::toFormat("uri=v_terminal;caption=;x=%d;y=%d;z=110;width=60;height=25;alignv=center;alignh=center;font-family=黑体;font-size=12;font-weight=normal;fill=#FFFFFF;",
			x,y).data(),
			"虚端子");

	}

	//如果当前IED后缀有A或B，且存储后缀为B或A的装置，认为是A/B套装置，自动添加切换按钮
	SString prex = sIedName.right(1);
	if(prex == "A" || prex == "B")
	{
		SString sIedName2 = sIedName.left(sIedName.length()-1);
		if(prex == "A")
			sIedName2 += "B";
		else
			sIedName2 += "A";
		stuSclVtIed *pIed2 = m_pSclVt->SearchIedByName(sIedName2.data());
		if(pIed2 != NULL)
		{
			stuSclVtIed *pIedA,*pIedB;
			if(prex == "A")
			{
				pIedA = pIed;
				pIedB = pIed2;
			}
			else
			{
				pIedA = pIed2;
				pIedB = pIed;
			}
			int x=180,y=6;
			SvgXml.AddChildNode("rect",SString::toFormat("x=%d;y=%d;z=100;width=40;height=25;stroke=#55AAFF;stroke-width=1;stroke-dasharray=0;fill=%s;",
				x,y,pIedA == pIed?"#0055FF":"none").data());
			SvgXml.AddChildNode("recttext",SString::toFormat("uri=ied.%s;caption=;x=%d;y=%d;z=110;width=40;height=25;alignv=center;alignh=center;font-family=黑体;font-size=12;font-weight=normal;fill=#FFFFFF;",
				pIedA->name,x,y).data(),
				"A套");
			
			x += 43;
			SvgXml.AddChildNode("rect",SString::toFormat("x=%d;y=%d;z=100;width=40;height=25;stroke=#55AAFF;stroke-width=1;stroke-dasharray=0;fill=%s;",
				x,y,pIedB == pIed?"#0055FF":"none").data());
			SvgXml.AddChildNode("recttext",SString::toFormat("uri=ied.%s;caption=;x=%d;y=%d;z=110;width=40;height=25;alignv=center;alignh=center;font-family=黑体;font-size=12;font-weight=normal;fill=#FFFFFF;",
				pIedB->name,x,y).data(),
				"B套");
		}
	}

	//绘制图例
	SBaseConfig *pGraphicCase = SvgXml.AddChildNode("g");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=791.350586;y=3.696384;z=100;width=204.799530;height=39.000000;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;fill=none;");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=856.704834;y=19.093992;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#BEBEBE;","静态配置");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=856.526855;y=34.607273;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#55AA00;","正常连接");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=943.041992;y=19.339748;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#FF0000;","数据无效");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=942.864014;y=34.853027;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#A76F00;","检修状态");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=796.670654;y=28.277107;z=100;font-family=宋体;font-size=14;font-weight=bold;fill=#FFFFFF;","图例");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=837.060669;y=25.460175;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#55AA00;");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=922.973022;y=9.946913;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#FF0000; ");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=922.973022;y=25.460175;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#A76F00;");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=837.060669;y=9.946913;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#AAAAAA; ");


// 	float cx = 120;
// 	float cy = 120;
// 	float a=0;
// 	float pi2 = 3.1415926/15;
// 	for(int i=0;i<15;i++)
// 	{
// 		float x1 = cx-100;
// 		float y1 = cy;
// 		float x2 = cx+100;
// 		float y2 = cy;
// 
// 		float x3 = (x1-cx)*cos(a)-(y1-cy)*sin(a)+cx;
// 		float x4 = (x2-cx)*cos(a)-(y2-cy)*sin(a)+cx;
// 		float y3 = (x1-cx)*sin(a)+(y1-cy)*cos(a)+cy;
// 		float y4 = (x2-cx)*sin(a)+(y2-cy)*cos(a)+cy;
// 		AddLine(&SvgXml,x3,y3,x4,y4,true,"z=100;stroke=#FFFFFF;stroke-width=2;stroke-dasharray=0;");
// 		AddLine(&SvgXml,x4,y4,x3,y3,true,"z=100;stroke=#FFFFFF;stroke-width=2;stroke-dasharray=0;");
// 		a += pi2;
// 	}
// 	return true;


	float x = 300;
	float y = -40;

	//以当前IED为中心
	//外框
	int ied_height=200;
	SBaseConfig *pIedBodyRect = SvgXml.AddChildNode("rect",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=0;width=161;height=200;stroke=#000000;stroke-width=1;stroke-dasharray=0;fill=#55aa7f;",
		pIed->name,x+7.118248,y+72.486839).data());
	//装置图片
	SvgXml.AddChildNode("image",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=100;width=145.988022;height=55.200756;imgsn=2;",
		pIed->name,x+14.937477,y+75.000526).data());
	SvgXml.AddChildNode("recttext",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=110;width=128.548660;height=47.667465;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#000000;",
		pIed->name,x+17.015774,y+76.963303).data(),
		SString::toFormat("%s\r\n%s",pIed->name,pIed->desc).data());

	//分析出当前IED的所有输入IED和输出IED，同时含有输入输出时放在输入IED列表中
	SPtrList<stuSclVtIedSmvOut> in_Smv;
	SPtrList<stuSclVtIedGooseOut> in_Goose;
	SPtrList<stuSclVtIed> in_ied,out_ied;
	stuSclVtIedSmvIn *pSmvIn = pIed->m_SmvIn.FetchFirst(pos1);
	while(pSmvIn)
	{
		if(pSmvIn->m_pIed != NULL && pSmvIn->m_pSmvOut != NULL)
		{
			in_Smv.append(pSmvIn->m_pSmvOut);
			if(!in_ied.exist(pSmvIn->m_pIed))
				in_ied.append(pSmvIn->m_pIed);
		}
		pSmvIn = pIed->m_SmvIn.FetchNext(pos1);
	}

	stuSclVtIedGooseIn *pGooseIn = pIed->m_GooseIn.FetchFirst(pos1);
	while(pGooseIn)
	{
		if(pGooseIn->m_pIed != NULL && pGooseIn->m_pGooseOut != NULL)
		{
			in_Goose.append(pGooseIn->m_pGooseOut);
			if(!in_ied.exist(pGooseIn->m_pIed))
				in_ied.append(pGooseIn->m_pIed);
		}
		pGooseIn = pIed->m_GooseIn.FetchNext(pos1);
	}

	//遍历所有IED，列出订阅当前IED中的控制块的IED
	SPtrList<stuSclVtIedSmvIn> out_Smv;
	SPtrList<stuSclVtIedGooseIn> out_Goose;
	stuSclVtIed *pIed2 = m_pSclVt->m_Ied.FetchFirst(pos1);
	while(pIed2)
	{
		bool bOk=false;
		pSmvIn = pIed2->m_SmvIn.FetchFirst(pos2);
		while(pSmvIn)
		{
			if(pSmvIn->m_pIed == pIed)
			{
				bOk = true;
				stuSclVtIedSmvOut *p = pIed->SearchSmvOutByNo(pSmvIn->ext_smv_no);
				if(p != NULL )
					out_Smv.append(pSmvIn);
			}
			pSmvIn = pIed2->m_SmvIn.FetchNext(pos2);
		}
		
		pGooseIn = pIed2->m_GooseIn.FetchFirst(pos2);
		while(pGooseIn)
		{
			if(pGooseIn->m_pIed == pIed)
			{
				bOk = true;
				stuSclVtIedGooseOut *p = pIed->SearchGooseOutByNo(pGooseIn->ext_gse_no);
				if(p != NULL)
					out_Goose.append(pGooseIn);
			}
			pGooseIn = pIed2->m_GooseIn.FetchNext(pos2);
		}

		if(bOk && !in_ied.exist(pIed2))
		{
			out_ied.append(pIed2);
		}

		pIed2 = m_pSclVt->m_Ied.FetchNext(pos1);
	}

	//对输入、输出IED按名称进行排序
	SPtrList<stuSclVtIed> tmp_ied;
	stuSclVtIed *pIedMin=NULL;
	while(in_ied.count()>0)
	{
		pIedMin = pIed2 = in_ied.FetchFirst(pos1);
		while(pIed2)
		{
			if(strcmp(pIed2->name,pIedMin->name) < 0)
				pIedMin = pIed2;
			pIed2 = in_ied.FetchNext(pos1);
		}
		tmp_ied.append(pIedMin);
		in_ied.remove(pIedMin);
	}
	//如果有MU输入，则将MU放在前面
	int smv_cnt=0;
	pIedMin = tmp_ied.FetchFirst(pos1);
	while(pIedMin)
	{
		stuSclVtIedSmvOut *pSmvOut = pIedMin->m_SmvOut.FetchFirst(pos2);
		while(pSmvOut)
		{
			if(in_Smv.exist(pSmvOut))
				break;
			pSmvOut = pIedMin->m_SmvOut.FetchNext(pos2);
		}
		if(pSmvOut != NULL)
			pIed2 = pIedMin;
		pIedMin = tmp_ied.FetchNext(pos1);
		if(pSmvOut != NULL)
		{
			in_ied.append(pIed2);
			tmp_ied.remove(pIed2);
			smv_cnt++;
		}
	}
	tmp_ied.copyto(in_ied);
	tmp_ied.clear();
	SPtrList<stuSclVtIed> out_ied_in;
	while(in_ied.count()>out_ied.count()+1)
	{
		if(in_ied.count()<= smv_cnt)
			break;
		//将GOOSEIN的IED移动到右侧
		pIedMin = in_ied[in_ied.count()-1];
		if(out_ied.count() == 0)
			out_ied.append(pIedMin);
		else
			out_ied.insertBefore(out_ied[0],pIedMin);
		out_ied_in.append(pIedMin);
		in_ied.remove(pIedMin);
	}

	pIedMin=NULL;
	while(out_ied.count()>0)
	{
		pIedMin = pIed2 = out_ied.FetchFirst(pos1);
		while(pIed2)
		{
			if(strcmp(pIed2->name,pIedMin->name) < 0)
				pIedMin = pIed2;
			pIed2 = out_ied.FetchNext(pos1);
		}
		tmp_ied.append(pIedMin);
		out_ied.remove(pIedMin);
	}
	tmp_ied.copyto(out_ied);
	
	//绘制输入IED及控制块
	int in_ied_y[128];
	memset(in_ied_y,0,sizeof(in_ied_y));
	int out_ied_y[128];
	memset(out_ied_y,0,sizeof(out_ied_y));
	pIed2 = in_ied.FetchFirst(pos1);
	x = 20;
	y = -30;
	int ied_idx=0;
	while(pIed2)
	{
		//外框
		SBaseConfig *pIed2BodyRect = SvgXml.AddChildNode("rect",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=0;width=161;height=200;stroke=#000000;stroke-width=1;stroke-dasharray=0;fill=#aaaa7f;",
			pIed2->name,x+7.118248,y+72.486839).data());
		//装置图片
		SvgXml.AddChildNode("image",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=100;width=145.988022;height=55.200756;imgsn=2;",
			pIed2->name,x+14.937477,y+75.000526).data());
		SvgXml.AddChildNode("recttext",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=110;width=128.548660;height=47.667465;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#000000;",
			pIed2->name,x+17.015774,y+76.963303).data(),
			SString::toFormat("%s\r\n%s",pIed2->name,pIed2->desc).data());
		in_ied_y[ied_idx++] = (int)(y+72.486839);

		//先SV后GOOSE
		float ap_y = 0;
		stuSclVtIedSmvOut *pSmvOut = pIed2->m_SmvOut.FetchFirst(pos2);
		while(pSmvOut)
		{
			if(!in_Smv.exist(pSmvOut))
			{
				pSmvOut = pIed2->m_SmvOut.FetchNext(pos2);
				continue;
			}
			//#005500
			sColor = GetCBColor(pSmvOut);
			SvgXml.AddChildNode("rect",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=99;width=150;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
				pIed2->name,pSmvOut->appid,x+13.447620,y+ap_y+131.240463,sColor.data(),sColor.data()).data());
			SvgXml.AddChildNode("image",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=100;width=16;height=16;imgsn=3;",
				pIed2->name,pSmvOut->appid,x+17.278427,y+ap_y+136.678741).data());
			SvgXml.AddChildNode("recttext",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=110;width=125.434517;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
				pIed2->name,pSmvOut->appid,x+34.322311,y+ap_y+132.430450).data(),
				SString::toFormat("SMV 0x%04X",pSmvOut->appid).data());
			sColor = GetCBLineColor(pSmvOut);
			//#FFFFFF
			AddLine(&SvgXml,x+160,y+ap_y+145,308,y+ap_y+145,true,
				SString::toFormat("uri=svin.%s$%d$%s;caption=;z=100;stroke=%s;stroke-width=2;stroke-dasharray=2;",
				pIed2->name,pSmvOut->smv_no,pIed->name,sColor.data()).data());


			ap_y += ap_height;
			pSmvOut = pIed2->m_SmvOut.FetchNext(pos2);
		}
		stuSclVtIedGooseOut *pGooseOut = pIed2->m_GooseOut.FetchFirst(pos2);
		while(pGooseOut)
		{
			if(!in_Goose.exist(pGooseOut))
			{
				pGooseOut = pIed2->m_GooseOut.FetchNext(pos2);
				continue;
			}
			//#7A0000
			sColor = GetCBColor(pGooseOut);
			SvgXml.AddChildNode("rect",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=99;width=150;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
				pIed2->name,pGooseOut->appid,x+13.447620,y+ap_y+131.240463,sColor.data(),sColor.data()).data());
			SvgXml.AddChildNode("image",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=100;width=16;height=16;imgsn=4;",
				pIed2->name,pGooseOut->appid,x+17.278427,y+ap_y+136.678741).data());
			SvgXml.AddChildNode("recttext",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=110;width=125.434517;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
				pIed2->name,pGooseOut->appid,x+34.322311,y+ap_y+132.430450).data(),
				SString::toFormat("GOOSE 0x%04X",pGooseOut->appid).data());
			//#FFFFFF
			sColor = GetCBLineColor(pGooseOut);
			AddLine(&SvgXml,x+160,y+ap_y+145,308,y+ap_y+145,true,
				SString::toFormat("uri=goin.%s$%d$%s;caption=;z=100;stroke=%s;stroke-width=2;stroke-dasharray=2;",
				pIed2->name,pGooseOut->gse_no,pIed->name,sColor.data()).data());

			ap_y += ap_height;
			pGooseOut = pIed2->m_GooseOut.FetchNext(pos2);
		}
		y += ap_y + 55 + 20;
		pIed2BodyRect->SetAttributeF("height",55+10+ap_y);
		if(y > ied_height)
			ied_height = (int)y;
		pIed2 = in_ied.FetchNext(pos1);
	}
	if(pIedBodyRect->GetAttributeF("height") < y+50)
		pIedBodyRect->SetAttributeI("height",y+50);

	//绘制输出IED及控制块
	pIed2 = out_ied.FetchFirst(pos1);
	x = 600;
	y = -30;
	ied_idx =0;
	while(pIed2)
	{
		//外框
		SBaseConfig *pIed2BodyRect = SvgXml.AddChildNode("rect",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=0;width=161;height=200;stroke=#000000;stroke-width=1;stroke-dasharray=0;fill=#aaaa7f;",
			pIed2->name,x+7.118248,y+72.486839).data());
		//装置图片
		SvgXml.AddChildNode("image",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=100;width=145.988022;height=55.200756;imgsn=2;",
			pIed2->name,x+14.937477,y+75.000526).data());
		SvgXml.AddChildNode("recttext",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=110;width=128.548660;height=47.667465;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#000000;",
			pIed2->name,x+17.015774,y+76.963303).data(),
			SString::toFormat("%s\r\n%s",pIed2->name,pIed2->desc).data());
		int ap_y = 0;
		if(out_ied_in.exist(pIed2))
		{
			stuSclVtIedGooseOut *pGooseOut = pIed2->m_GooseOut.FetchFirst(pos2);
			while(pGooseOut)
			{
				if(!in_Goose.exist(pGooseOut))
				{
					pGooseOut = pIed2->m_GooseOut.FetchNext(pos2);
					continue;
				}
				//#7A0000
				sColor = GetCBColor(pGooseOut);
				SvgXml.AddChildNode("rect",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=99;width=150;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
					pIed2->name,pGooseOut->appid,x+13.447620,y+ap_y+131.240463,sColor.data(),sColor.data()).data());
				SvgXml.AddChildNode("image",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=100;width=16;height=16;imgsn=4;",
					pIed2->name,pGooseOut->appid,x+17.278427,y+ap_y+136.678741).data());
				SvgXml.AddChildNode("recttext",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=110;width=125.434517;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
					pIed2->name,pGooseOut->appid,x+34.322311,y+ap_y+132.430450).data(),
					SString::toFormat("GOOSE 0x%04X",pGooseOut->appid).data());
				//#FFFFFF
				sColor = GetCBLineColor(pGooseOut);
				AddLine(&SvgXml,x+16,y+ap_y+143,468,y+ap_y+143,true,
					SString::toFormat("uri=goin.%s$%d$%s;caption=;z=100;stroke=%s;stroke-width=2;stroke-dasharray=2;",
					pIed2->name,pGooseOut->gse_no,pIed->name,sColor.data()).data());

				ap_y += ap_height;
				pGooseOut = pIed2->m_GooseOut.FetchNext(pos2);
			}
		}

		out_ied_y[ied_idx++] = (int)(y+72.486839);
		y += ap_y + 55 + 20;
		pIed2BodyRect->SetAttributeF("height",55+10+ap_y);
		if(y > ied_height)
			ied_height = (int)y;
		pIed2 = out_ied.FetchNext(pos1);
	}
	if(pIedBodyRect->GetAttributeF("height") < y+50)
		pIedBodyRect->SetAttributeI("height",y+50);

	//计算当前主IED的控制块总数
	int cb_cnt = pIed->m_SmvOut.count() + pIed->m_GooseOut.count();
	
	if(pIedBodyRect->GetAttributeF("height") < ap_height * cb_cnt + 60)
		pIedBodyRect->SetAttributeI("height",ap_height * cb_cnt + 60);

	int ied_ap_height = pIedBodyRect->GetAttributeF("height") - 60;
	if(cb_cnt > 0)
		ied_ap_height = ied_ap_height/cb_cnt;
	x = 300;
	y = -40+ied_ap_height/2;

	//先输入后输出
	//先SV后GOOSE
	float ap_y = 0;
	stuSclVtIedSmvOut *pSmvOut = pIed->m_SmvOut.FetchFirst(pos1);
	while(pSmvOut)
	{
		//#005500
		sColor = GetCBColor(pSmvOut);
		SvgXml.AddChildNode("rect",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=10;width=150;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
			pIed->name,pSmvOut->appid,x+13.447620,y+ap_y+131.240463,sColor.data(),sColor.data()).data());
		SvgXml.AddChildNode("image",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=100;width=16;height=16;imgsn=3;",
			pIed->name,pSmvOut->appid,x+17.278427,y+ap_y+136.678741).data());
		SvgXml.AddChildNode("recttext",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=110;width=125.434517;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
			pIed->name,pSmvOut->appid,x+34.322311,y+ap_y+132.430450).data(),
			SString::toFormat("SMV 0x%04X",pSmvOut->appid).data());
		//绘制输出线段，带箭头
		//先选择输入IED中的
		//#FFFF7F
		sColor = GetCBLineColor(pSmvOut);
		pIed2 = in_ied.FetchFirst(pos2);
		ied_idx = 0;
		while(pIed2)
		{
			if(pIed2->SearchSmvInByExtNo(pSmvOut->ied_name.data(),pSmvOut->smv_no) != NULL)
			{
				//有订阅关系
				AddLine(&SvgXml,x+15,y+ap_y+145,188,in_ied_y[ied_idx]+10,true,
					SString::toFormat("uri=svin.%s$%d$%s;caption=;z=255;stroke=%s;stroke-width=2;stroke-dasharray=0;",
					pIed->name,pSmvOut->smv_no,pIed2->name,sColor.data()).data());
			}
			pIed2 = in_ied.FetchNext(pos2);
			ied_idx++;
		}
		ap_y += ied_ap_height;
		pSmvOut = pIed->m_SmvOut.FetchNext(pos1);
	}
	stuSclVtIedGooseOut *pGooseOut = pIed->m_GooseOut.FetchFirst(pos1);
	while(pGooseOut)
	{
		//#7A0000
		sColor = GetCBColor(pGooseOut);
		SvgXml.AddChildNode("rect",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=10;width=150;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
			pIed->name,pGooseOut->appid,x+13.447620,y+ap_y+131.240463,sColor.data(),sColor.data()).data());
		SvgXml.AddChildNode("image",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=100;width=16;height=16;imgsn=4;",
			pIed->name,pGooseOut->appid,x+17.278427,y+ap_y+136.678741).data());
		SvgXml.AddChildNode("recttext",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=110;width=125.434517;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
			pIed->name,pGooseOut->appid,x+34.322311,y+ap_y+132.430450).data(),
			SString::toFormat("GOOSE 0x%04X",pGooseOut->appid).data());
		//#FFFF7F
		sColor = GetCBLineColor(pGooseOut);
		//绘制输出线段，带箭头
		//先选择输入IED中的
		pIed2 = in_ied.FetchFirst(pos2);
		ied_idx = 0;
		while(pIed2)
		{
			if(pIed2->SearchGooseInByExtNo(pGooseOut->ied_name.data(),pGooseOut->gse_no) != NULL)
			{
				//有订阅关系
				AddLine(&SvgXml,x+15,y+ap_y+145,188,in_ied_y[ied_idx]+10,true,
					SString::toFormat("uri=goin.%s$%d$%s;caption=;z=255;stroke=%s;stroke-width=2;stroke-dasharray=0;",
					pIed->name,pGooseOut->gse_no,pIed2->name,sColor.data()).data());
			}
			pIed2 = in_ied.FetchNext(pos2);
			ied_idx++;
		}
		ap_y += ied_ap_height;
		pGooseOut = pIed->m_GooseOut.FetchNext(pos1);
	}
	
	m_iVLineX.clear();
	m_iHLineY.clear();
	//右边部分
	ap_y = 0;
	pSmvOut = pIed->m_SmvOut.FetchFirst(pos1);
	while(pSmvOut)
	{
		//绘制输出线段，带箭头
		//先选择输入IED中的
		//#FFFF7F
		sColor = GetCBLineColor(pSmvOut);
		pIed2 = out_ied.FetchFirst(pos2);
		ied_idx = 0;
		while(pIed2)
		{
			if(pIed2->SearchSmvInByExtNo(pSmvOut->ied_name.data(),pSmvOut->smv_no) != NULL)
			{
				//有订阅关系
				AddLine(&SvgXml,x+13+145,y+ap_y+145,608,out_ied_y[ied_idx]+10,true,
					SString::toFormat("uri=svin.%s$%d$%s;caption=;z=255;stroke=%s;stroke-width=2;stroke-dasharray=0;",
					pIed->name,pSmvOut->smv_no,pIed2->name,sColor.data()).data());
			}
			pIed2 = out_ied.FetchNext(pos2);
			ied_idx++;
		}
		ap_y += ied_ap_height;
		pSmvOut = pIed->m_SmvOut.FetchNext(pos1);
	}
	pGooseOut = pIed->m_GooseOut.FetchFirst(pos1);
	while(pGooseOut)
	{
		//绘制输出线段，带箭头
		//先选择输入IED中的
		//#FFFF7F
		sColor = GetCBLineColor(pGooseOut);
		pIed2 = out_ied.FetchFirst(pos2);
		ied_idx = 0;
		while(pIed2)
		{
			if(pIed2->SearchGooseInByExtNo(pGooseOut->ied_name.data(),pGooseOut->gse_no) != NULL)
			{
				//有订阅关系
				AddLine(&SvgXml,x+13+145,y+ap_y+145,608,out_ied_y[ied_idx]+10,true,
					SString::toFormat("uri=goin.%s$%d$%s;caption=;z=255;stroke=%s;stroke-width=2;stroke-dasharray=0;",
					pIed->name,pGooseOut->gse_no,pIed2->name,sColor.data()).data());
			}
			pIed2 = out_ied.FetchNext(pos2);
			ied_idx++;
		}
		ap_y += ied_ap_height;
		pGooseOut = pIed->m_GooseOut.FetchNext(pos1);
	}
	y += ap_y + 55 + 20;


	pWndRect->SetAttributeF("height",pIedBodyRect->GetAttributeF("height")+50);
	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  生成指定IED的出、入控制块所有虚端子连接图，以指定IED为中心
// 作    者:  邵凯田
// 创建时间:  2016-1-13 17:00
// 参数说明:  @sIedName表示IED名称，""表示第一个IED
// 返 回 值:  false/true
//////////////////////////////////////////////////////////////////////////
bool CSpVtSvgGenerator::GeneratorIedVirtualTerminal(SString sIedName,SXmlConfig &SvgXml)
{
	if(m_pSclVt == NULL)
		return false;
	m_bShowVT = true;
	m_HisLines.clear();
	m_iHLineY.clear();
	m_iVLineX.clear();
	m_RelaIeds.clear();
	unsigned long pos1,pos2,pos3;
	stuSclVtIed *pIed = (sIedName.length() == 0)?(m_pSclVt->m_Ied[0]):m_pSclVt->SearchIedByName(sIedName.data());
	if(pIed == NULL)
		return false;

	SvgXml.clear();
	SvgXml.SetNodeName("svg");
	SBaseConfig *pDefs = SvgXml.AddChildNode("defs");
	int ap_height=30;//一个访问点控制块的高度
	//SBaseConfig *pImagedef;

	pDefs->AddChildNode("image_def","sn=1;width=128;height=128;",g_p_vt_image_hexstr_home);
	pDefs->AddChildNode("image_def","sn=2;width=100;height=30;",g_p_vt_image_hexstr_ied);
	pDefs->AddChildNode("image_def","sn=3;width=16;height=16;",g_p_vt_image_hexstr_sv);
	pDefs->AddChildNode("image_def","sn=4;width=16;height=16;",g_p_vt_image_hexstr_goose);
	pDefs->AddChildNode("image_def","sn=5;width=16;height=16;",g_p_vt_image_hexstr_mms);

	//float fWndHeight=600;
	SBaseConfig *pWndRect = SvgXml.AddChildNode("rect","uri=;caption=;x=0;y=0;z=;width=1280;height=600;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;fill=#000000;");
	SvgXml.AddChildNode("image","uri=home;caption=;x=10;y=6;z=100;width=24;height=24;imgsn=1;");

	//显示虚端子及虚链路
	{
		int x=40,y=6;
		SvgXml.AddChildNode("rect",SString::toFormat("x=%d;y=%d;z=100;width=60;height=25;stroke=#55AAFF;stroke-width=1;stroke-dasharray=0;fill=%s;",
			x,y,m_bShowVT==false?"#0055FF":"none").data());
		SvgXml.AddChildNode("recttext",SString::toFormat("uri=v_line;caption=;x=%d;y=%d;z=110;width=60;height=25;alignv=center;alignh=center;font-family=黑体;font-size=12;font-weight=normal;fill=#FFFFFF;",
			x,y).data(),
			"虚链路");

		x += 63;
		SvgXml.AddChildNode("rect",SString::toFormat("x=%d;y=%d;z=100;width=60;height=25;stroke=#55AAFF;stroke-width=1;stroke-dasharray=0;fill=%s;",
			x,y,m_bShowVT==true?"#0055FF":"none").data());
		SvgXml.AddChildNode("recttext",SString::toFormat("uri=v_terminal;caption=;x=%d;y=%d;z=110;width=60;height=25;alignv=center;alignh=center;font-family=黑体;font-size=12;font-weight=normal;fill=#FFFFFF;",
			x,y).data(),
			"虚端子");

	}


	//如果当前IED后缀有A或B，且存储后缀为B或A的装置，认为是A/B套装置，自动添加切换按钮
	SString prex = sIedName.right(1);
	if(prex == "A" || prex == "B")
	{
		SString sIedName2 = sIedName.left(sIedName.length()-1);
		if(prex == "A")
			sIedName2 += "B";
		else
			sIedName2 += "A";
		stuSclVtIed *pIed2 = m_pSclVt->SearchIedByName(sIedName2.data());
		if(pIed2 != NULL)
		{
			stuSclVtIed *pIedA,*pIedB;
			if(prex == "A")
			{
				pIedA = pIed;
				pIedB = pIed2;
			}
			else
			{
				pIedA = pIed2;
				pIedB = pIed;
			}
			int x=180,y=6;
			SvgXml.AddChildNode("rect",SString::toFormat("x=%d;y=%d;z=100;width=40;height=25;stroke=#55AAFF;stroke-width=1;stroke-dasharray=0;fill=%s;",
				x,y,pIedA == pIed?"#0055FF":"none").data());
			SvgXml.AddChildNode("recttext",SString::toFormat("uri=ied.%s;caption=;x=%d;y=%d;z=110;width=40;height=25;alignv=center;alignh=center;font-family=黑体;font-size=12;font-weight=normal;fill=#FFFFFF;",
				pIedA->name,x,y).data(),
				"A套");

			x += 43;
			SvgXml.AddChildNode("rect",SString::toFormat("x=%d;y=%d;z=100;width=40;height=25;stroke=#55AAFF;stroke-width=1;stroke-dasharray=0;fill=%s;",
				x,y,pIedB == pIed?"#0055FF":"none").data());
			SvgXml.AddChildNode("recttext",SString::toFormat("uri=ied.%s;caption=;x=%d;y=%d;z=110;width=40;height=25;alignv=center;alignh=center;font-family=黑体;font-size=12;font-weight=normal;fill=#FFFFFF;",
				pIedB->name,x,y).data(),
				"B套");
		}
	}

	//绘制图例
	SBaseConfig *pGraphicCase = SvgXml.AddChildNode("g");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=791.350586;y=3.696384;z=100;width=204.799530;height=39.000000;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;fill=none;");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=856.704834;y=19.093992;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#BEBEBE;","静态配置");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=856.526855;y=34.607273;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#55AA00;","正常连接");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=943.041992;y=19.339748;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#FF0000;","数据无效");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=942.864014;y=34.853027;z=100;font-family=宋体;font-size=11;font-weight=normal;fill=#A76F00;","检修状态");
	pGraphicCase->AddChildNode("text","uri=;caption=;x=796.670654;y=28.277107;z=100;font-family=宋体;font-size=14;font-weight=bold;fill=#FFFFFF;","图例");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=837.060669;y=25.460175;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#55AA00;");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=922.973022;y=9.946913;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#FF0000; ");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=922.973022;y=25.460175;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#A76F00;");
	pGraphicCase->AddChildNode("rect","uri=;caption=;x=837.060669;y=9.946913;z=100;width=10;height=10;stroke=#666666;stroke-width=1;stroke-dasharray=0;fill=#AAAAAA; ");


	//分析出当前IED的所有输入、输出关联的IED，并将含SMV的输入的设备放在前面，所有设备按名称排序（升序）
	stuSvgVtCbIed *pSvgIed;
	stuSvgVtCbIedItem *pSvgIedItem;
	//SMV 输入
	stuSclVtIedSmvIn *pSmvIn = pIed->m_SmvIn.FetchFirst(pos1);
	while(pSmvIn)
	{
		if(pSmvIn->m_pIed != NULL && pSmvIn->m_pSmvOut != NULL)
		{
			pSvgIed = SearchRelaIed(pSmvIn->m_pIed);
			pSvgIedItem = new stuSvgVtCbIedItem();
			pSvgIedItem->type = 1;
			pSvgIedItem->pRelaIed = pSmvIn->m_pIed;
			pSvgIedItem->pSmvOut =  pSmvIn->m_pSmvOut;
			pSvgIedItem->pSmvIn =  pSmvIn;
			pSvgIed->items.append(pSvgIedItem);
		}
		pSmvIn = pIed->m_SmvIn.FetchNext(pos1);
	}
	//GOOSE输入
	stuSclVtIedGooseIn *pGooseIn = pIed->m_GooseIn.FetchFirst(pos1);
	while(pGooseIn)
	{
		if(pGooseIn->m_pIed != NULL && pGooseIn->m_pGooseOut != NULL)
		{
			pSvgIed = SearchRelaIed(pGooseIn->m_pIed);
			pSvgIedItem = new stuSvgVtCbIedItem();
			pSvgIedItem->type = 2;
			pSvgIedItem->pRelaIed = pGooseIn->m_pIed;
			pSvgIedItem->pGooseOut =  pGooseIn->m_pGooseOut;
			pSvgIedItem->pGooseIn =  pGooseIn;
			pSvgIed->items.append(pSvgIedItem);
		}
		pGooseIn = pIed->m_GooseIn.FetchNext(pos1);
	}
	//遍历所有IED，列出订阅当前IED中的控制块的IED
	stuSclVtIed *pIed2 = m_pSclVt->m_Ied.FetchFirst(pos1);
	while(pIed2)
	{
		pSmvIn = pIed2->m_SmvIn.FetchFirst(pos2);
		while(pSmvIn)
		{
			if(pSmvIn->m_pIed == pIed)
			{
				//SMV OUT
				pSvgIed = SearchRelaIed(pIed2);
				pSvgIedItem = new stuSvgVtCbIedItem();
				pSvgIedItem->type = 3;
				pSvgIedItem->pRelaIed = pIed2;
				pSvgIedItem->pSmvOut =  pSmvIn->m_pSmvOut;
				pSvgIedItem->pSmvIn =  pSmvIn;
				pSvgIed->items.append(pSvgIedItem);
			}
			pSmvIn = pIed2->m_SmvIn.FetchNext(pos2);
		}

		pGooseIn = pIed2->m_GooseIn.FetchFirst(pos2);
		while(pGooseIn)
		{
			if(pGooseIn->m_pIed == pIed)
			{
				pSvgIed = SearchRelaIed(pIed2);
				pSvgIedItem = new stuSvgVtCbIedItem();
				pSvgIedItem->type = 4;
				pSvgIedItem->pRelaIed = pIed2;
				pSvgIedItem->pGooseOut =  pGooseIn->m_pGooseOut;
				pSvgIedItem->pGooseIn =  pGooseIn;
				pSvgIed->items.append(pSvgIedItem);
			}
			pGooseIn = pIed2->m_GooseIn.FetchNext(pos2);
		}
		pIed2 = m_pSclVt->m_Ied.FetchNext(pos1);
	}
	SortRelaIed();

	//计算各IED高度的总高度
	int all_ied_height = 0;
	int ied_head_height=60;
	int vt_channel_height=15;
	pSvgIed = m_RelaIeds.FetchFirst(pos1);
	while(pSvgIed)
	{
		pSvgIed->ied_height = ied_head_height;
		int cb_cnt = 0;//主动发出的块数量
		stuSvgVtCbIedItem *p = pSvgIed->items.FetchFirst(pos2);
		while(p)
		{
			if(p->type == 1)
			{
				//smv in
				cb_cnt++;
				int h = vt_channel_height*p->pSmvIn->m_Channel.count();
				if(h<ap_height)
					h = ap_height;
				pSvgIed->ied_height += h;
			}
			else if(p->type == 3)
			{
				//smv out
				int h = vt_channel_height*p->pSmvIn->m_Channel.count();
				if(h<ap_height)
					h = ap_height;
				pSvgIed->ied_height += h;
			}
			else if(p->type == 2)
			{
				//gse in
				cb_cnt ++;
				int h = vt_channel_height*p->pGooseIn->m_Channel.count();
				if(h<ap_height)
					h = ap_height;
				pSvgIed->ied_height += h;
			}
			else if(p->type == 4)
			{
				//gse out 
				int h = vt_channel_height*p->pGooseIn->m_Channel.count();
				if(h<ap_height)
					h = ap_height;
				pSvgIed->ied_height += h;
			}
			pSvgIed->ied_height += vt_channel_height;
			p = pSvgIed->items.FetchNext(pos2);
		}
		if(pSvgIed->ied_height < ied_head_height+ap_height*cb_cnt)
			pSvgIed->ied_height = ied_head_height+ap_height*cb_cnt;
		all_ied_height += pSvgIed->ied_height;
		pSvgIed = m_RelaIeds.FetchNext(pos1);
	}

	//
	float x = 560;
	float y = -40;
	//以当前IED为中心
	//外框
	//int ied_height=200;
	SBaseConfig *pIedBodyRect = SvgXml.AddChildNode("rect",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=0;width=161;height=200;stroke=#000000;stroke-width=1;stroke-dasharray=0;fill=#55aa7f;",
		pIed->name,x+7.118248,y+72.486839).data());
	//装置图片
	SvgXml.AddChildNode("image",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=100;width=145.988022;height=55.200756;imgsn=2;",
		pIed->name,x+14.937477,y+75.000526).data());
	SvgXml.AddChildNode("recttext",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=110;width=128.548660;height=47.667465;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#000000;",
		pIed->name,x+17.015774,y+76.963303).data(),
		SString::toFormat("%s\r\n%s",pIed->name,pIed->desc).data());

	//分左右两次绘制，SMVIN固定放在左侧，剩余的左右两侧按Y方向平分
	//bool bHaveSmv=false;
	SString sColor;
	pSvgIed = m_RelaIeds.FetchFirst(pos1);
	for(int i=0;i<2;i++)
	{
		if(i == 0)
		{
			x = 10;
		}
		else
		{
			x = 1110;
		}
		y = -40;
		bool bLeftToRight;//是否从左向右的方向
		while(pSvgIed)
		{
			pIed2 = pSvgIed->pRelaIed;
			//绘制关联IED的图标
			//外框
			SBaseConfig *pIed2BodyRect = SvgXml.AddChildNode("rect",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=10;width=161;height=200;stroke=#000000;stroke-width=1;stroke-dasharray=0;fill=#aaaa7f;",
				pIed2->name,x+7.118248,y+72.486839).data());
			//装置图片
			SvgXml.AddChildNode("image",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=100;width=145.988022;height=55.200756;imgsn=2;",
				pIed2->name,x+14.937477,y+75.000526).data());
			SvgXml.AddChildNode("recttext",SString::toFormat("uri=ied.%s;caption=;x=%f;y=%f;z=110;width=128.548660;height=47.667465;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#000000;",
				pIed2->name,x+17.015774,y+76.963303).data(),
				SString::toFormat("%s\r\n%s",pIed2->name,pIed2->desc).data());
			

			//pSvgIed->ied_height = 0;
			//int cb_cnt = 0;//主动发出的块数量
			float h = 0;
			float h0=0;
			float y0=ied_head_height+55;
			stuSclVtIedDatasetFcda *pFcda;
			stuSvgVtCbIedItem *p = pSvgIed->items.FetchFirst(pos2);
			while(p)
			{
				if(p->type == 1)
				{ 
					//smv in
					h0 = h = vt_channel_height*p->pSmvIn->m_Channel.count();
					if(h<ap_height)
						h = ap_height;
					y0 += h/2;
					//y += h/2;
					sColor = GetCBColor(p->pSmvOut);
					SvgXml.AddChildNode("rect",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=101;width=150;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
						pIed2->name,p->pSmvOut->appid,x+13.447620,y+y0/*+ap_y*/+131.240463-130,sColor.data(),sColor.data()).data());
					SvgXml.AddChildNode("image",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=110;width=16;height=16;imgsn=3;",
						pIed2->name,p->pSmvOut->appid,x+17.278427,y+y0/*+ap_y*/+136.678741-130).data());
					SvgXml.AddChildNode("recttext",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=110;width=125.434517;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
						pIed2->name,p->pSmvOut->appid,x+34.322311,y+y0/*+ap_y*/+132.430450-130).data(),
						SString::toFormat("SMV 0x%04X",p->pSmvOut->appid).data());
				}
				else if(p->type == 3)
				{ 
					//smv out
					h0 = h = vt_channel_height*p->pSmvIn->m_Channel.count();
					if(h<ap_height)
						h = ap_height;
					y0 += h/2;
					//y += h/2;
					sColor = GetCBColor(p->pSmvOut);
					SvgXml.AddChildNode("rect",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=101;width=70;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
						pIed->name,p->pSmvOut->appid,(i==0?0:83)+558+13.447620,y+y0/*+ap_y*/+131.240463-130,sColor.data(),sColor.data()).data());
					SvgXml.AddChildNode("image",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=110;width=16;height=16;imgsn=3;",
						pIed->name,p->pSmvOut->appid,(i==0?0:83)+558+17.278427,y+y0/*+ap_y*/+136.678741-130).data());
					SvgXml.AddChildNode("recttext",SString::toFormat("uri=svcb.%s$0x%04X;caption=;x=%f;y=%f;z=110;width=50;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
						pIed->name,p->pSmvOut->appid,(i==0?0:83)+558+34.322311,y+y0/*+ap_y*/+132.430450-130).data(),
						SString::toFormat("0x%04X",p->pSmvOut->appid).data());
				}
				else if(p->type == 2)
				{
					//gse in
					h0 = h = vt_channel_height*p->pGooseIn->m_Channel.count();
					if(h<ap_height)
						h = ap_height;
					//y += h/2;
					y0 += h/2;
					sColor = GetCBColor(p->pGooseOut);
					SvgXml.AddChildNode("rect",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=101;width=150;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
						pIed2->name,p->pGooseOut->appid,x+13.447620,y+y0/*+ap_y*/+131.240463-130,sColor.data(),sColor.data()).data());
					SvgXml.AddChildNode("image",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=110;width=16;height=16;imgsn=4;",
						pIed2->name,p->pGooseOut->appid,x+17.278427,y+y0/*+ap_y*/+136.678741-130).data());
					SvgXml.AddChildNode("recttext",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=110;width=125.434517;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
						pIed2->name,p->pGooseOut->appid,x+34.322311,y+y0/*+ap_y*/+132.430450-130).data(),
						SString::toFormat("GOOSE 0x%04X",p->pGooseOut->appid).data());
				}
				else if(p->type == 4)
				{
					//gse out
					h0 = h = vt_channel_height*p->pGooseIn->m_Channel.count();
					if(h<ap_height)
						h = ap_height;
					//y += h/2;
					y0 += h/2;
					sColor = GetCBColor(p->pGooseOut);
					SvgXml.AddChildNode("rect",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=99;width=70;height=25;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
						pIed->name,p->pGooseOut->appid,(i==0?0:83)+558+13.447620,y+y0/*+ap_y*/+131.240463-130,sColor.data(),sColor.data()).data());
					SvgXml.AddChildNode("image",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=100;width=16;height=16;imgsn=4;",
						pIed->name,p->pGooseOut->appid,(i==0?0:83)+558+17.278427,y+y0/*+ap_y*/+136.678741-130).data());
					SvgXml.AddChildNode("recttext",SString::toFormat("uri=gocb.%s$0x%04X;caption=;x=%f;y=%f;z=100;width=50;height=22.802647;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
						pIed->name,p->pGooseOut->appid,(i==0?0:83)+558+34.322311,y+y0/*+ap_y*/+132.430450-130).data(),
						SString::toFormat("0x%04X",p->pGooseOut->appid).data());
				}
				//在y+y0的位置绘制所有订阅通道
				float vx1,vx2,vy;
				vx1 = (i==0?178:728);
				vx2 = (i==0?567:1116);
				vy = y+y0+20-h/2+vt_channel_height/2;
				if(h0-vt_channel_height <= 0)
					vy += 7;
				if(p->pSmvOut==NULL)
					sColor = GetCBLineColor(p->pGooseOut);
				else
					sColor = GetCBLineColor(p->pSmvOut);
				//从右开始的起始线段
				if((i==0 && (p->type == 3 || p->type == 4)) ||//从左向右箭头
					(i==1 && (p->type == 1|| p->type == 2)))
				{
					bLeftToRight = true;
					AddLine(&SvgXml,vx1+50,vy+h0/2-vt_channel_height,vx1,vy+h0/2-vt_channel_height,true,
					SString::toFormat("uri=%s.%s$0x%d$%s;z=100;stroke=%s;stroke-width=1;stroke-dasharray=0;",
					(p->type==1||p->type==3)?"svin":"goin",(p->type==1||p->type==3)?p->pSmvIn->m_pIed->name:p->pGooseIn->m_pIed->name,(p->type==1||p->type==3)?p->pSmvOut->smv_no:p->pGooseOut->gse_no,(p->type==1||p->type==2)?pIed->name:pSvgIed->pRelaIed->name,sColor.data()).data());
				}
				else
				{
					bLeftToRight = false;
					AddLine(&SvgXml,vx1,vy+h0/2-vt_channel_height,vx1+50,vy+h0/2-vt_channel_height,false,
					SString::toFormat("uri=%s.%s$0x%d$%s;z=100;stroke=%s;stroke-width=1;stroke-dasharray=0;",
					(p->type==1||p->type==3)?"svin":"goin",(p->type==1||p->type==3)?p->pSmvIn->m_pIed->name:p->pGooseIn->m_pIed->name,(p->type==1||p->type==3)?p->pSmvOut->smv_no:p->pGooseOut->gse_no,(p->type==1||p->type==2)?pIed->name:pSvgIed->pRelaIed->name,sColor.data()).data());
				}
				//if(h0-vt_channel_height > 0)
				{
					//左垂直线
					AddLine(&SvgXml,vx1+50,vy-vt_channel_height,vx1+50,vy+h0-vt_channel_height,false,SString::toFormat("z=100;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;"/*,sColor.data()*/).data());
					//右垂直线
					AddLine(&SvgXml,vx2-50,vy-vt_channel_height,vx2-50,vy+h0-vt_channel_height,false,SString::toFormat("z=100;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;"/*,sColor.data()*/).data());
				}
				//右侧结束线段
				if(bLeftToRight)//从左向右箭头
					AddLine(&SvgXml,vx2-50,vy+h0/2-vt_channel_height,vx2,vy+h0/2-vt_channel_height,false,
					SString::toFormat("uri=%s.%s$0x%d$%s;z=100;stroke=%s;stroke-width=1;stroke-dasharray=0;",
					(p->type==1||p->type==3)?"svin":"goin",(p->type==1||p->type==3)?p->pSmvIn->m_pIed->name:p->pGooseIn->m_pIed->name,(p->type==1||p->type==3)?p->pSmvOut->smv_no:p->pGooseOut->gse_no,(p->type==1||p->type==2)?pIed->name:pSvgIed->pRelaIed->name,sColor.data()).data());
				else
					AddLine(&SvgXml,vx2-50,vy+h0/2-vt_channel_height,vx2,vy+h0/2-vt_channel_height,true,
					SString::toFormat("uri=%s.%s$0x%d$%s;z=100;stroke=%s;stroke-width=1;stroke-dasharray=0;",
					(p->type==1||p->type==3)?"svin":"goin",(p->type==1||p->type==3)?p->pSmvIn->m_pIed->name:p->pGooseIn->m_pIed->name,(p->type==1||p->type==3)?p->pSmvOut->smv_no:p->pGooseOut->gse_no,(p->type==1||p->type==2)?pIed->name:pSvgIed->pRelaIed->name,sColor.data()).data());
				if(h0-vt_channel_height <= 0)
					vy = vy/*-h/2*/;
				if(p->type == 1 || p->type == 3)
				{
					stuSclVtIedSmvInChannel *pChn = p->pSmvIn->m_Channel.FetchFirst(pos3);
					while(pChn)
					{
						sColor = GetCBLineColor(p->pSmvOut,pChn);
						if(sColor != "#FFFFFF" && sColor !="#55FF00")
						{
// 							if(sColor == "#55FF00")
// 								sColor = "#061400";
							SvgXml.AddChildNode("rect",SString::toFormat("uri=;caption=;x=%f;y=%f;z=99;width=%f;height=13;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
								vx1+51,vy-vt_channel_height+1,vx2-vx1-102,sColor.data(),sColor.data()).data());
						}

						AddLine(&SvgXml,vx1+50,vy-vt_channel_height,vx2-50,vy-vt_channel_height,false,SString::toFormat("z=100;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;"/*,sColor.data()*/).data());

						const char* desc = NULL;
						pFcda = pChn->m_pFcda;
						if(pFcda != NULL)
						{
							desc = pFcda->dai_desc;
							if(desc == NULL || desc[0] == '\0')
								desc = pFcda->doi_desc;
							if(desc == NULL || desc[0] == '\0')
								desc = pFcda->ln_desc;
						}
						if(desc == NULL)
							desc = "";
						SString fcda_desc,chn_desc;
						if(pFcda == NULL)
							fcda_desc = "【尚未连接】";
						else
							fcda_desc.sprintf("%2d:%s",pFcda->chn_no,desc);
						if(fcda_desc.length() > 22)
						{
							fcda_desc = fcda_desc.left(22);
							char *pStr = fcda_desc.data();
							bool bChn=false;
							while(*pStr != '\0')
							{
								if(bChn)
									bChn = false;
								else if((BYTE)*pStr >= 0x80)
									bChn = true;
								pStr++;
							}
							if(bChn)
								pStr[-1] = '\0';
							fcda_desc = fcda_desc+" ...";
						}
						chn_desc.sprintf("%2d:%s",pChn->int_chn_no,pChn->int_chn_desc);
						if(chn_desc.length() > 22)
						{
							chn_desc = chn_desc.left(22);
							char *pStr = chn_desc.data();
							bool bChn=false;
							while(*pStr != '\0')
							{
								if(bChn)
									bChn = false;
								else if((BYTE)*pStr >= 0x80)
									bChn = true;
								pStr++;
							}
							if(bChn)
								pStr[-1] = '\0';
							chn_desc = chn_desc+" ...";
							//chn_desc = chn_desc.left(22)+" ...";
						}

						SvgXml.AddChildNode("recttext",SString::toFormat("uri=%s;caption=;x=%f;y=%f;z=110;width=%d;height=%d;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
							p->type==1?SString::toFormat("svchin.%s$%d$%s$%d",pSvgIed->pRelaIed->name,p->pSmvOut->smv_no,pIed->name,pChn->int_chn_no).data():
							SString::toFormat("svchin.%s$%d$%s$%d",pIed->name,p->pSmvOut->smv_no,pSvgIed->pRelaIed->name,pChn->int_chn_no).data(),
							(vx1+vx2)/2-145,vy-vt_channel_height,290,vt_channel_height).data(),
							SString::toFormat("%s",bLeftToRight?"<--":"-->").data());
// 						SvgXml.AddChildNode("recttext",SString::toFormat("uri=;caption=;x=%f;y=%f;z=110;width=%d;height=%d;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
// 							(vx1+vx2)/2-20,vy-vt_channel_height,40,vt_channel_height).data(),
// 							SString::toFormat("%s",bLeftToRight?"<--":"-->").data());
						SvgXml.AddChildNode("recttext",SString::toFormat("uri=;caption=;x=%f;y=%f;z=110;width=%f;height=%d;alignv=center;alignh=right;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
							vx1,vy-vt_channel_height,(vx1+vx2)/2-(vx1)-15,vt_channel_height).data(),
							SString::toFormat("%s",!bLeftToRight?fcda_desc.data():chn_desc.data()).data());
						SvgXml.AddChildNode("recttext",SString::toFormat("uri=;caption=;x=%f;y=%f;z=110;width=%f;height=%d;alignv=center;alignh=left;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
							(vx1+vx2)/2+15,vy-vt_channel_height,(vx2)-(vx1+vx2)/2-20,vt_channel_height).data(),
							SString::toFormat("%s",bLeftToRight?fcda_desc.data():chn_desc.data()).data());
// 						SvgXml.AddChildNode("recttext",SString::toFormat("uri=;caption=;x=%f;y=%f;z=110;width=%f;height=%d;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
// 							vx1+50,vy-vt_channel_height,(vx2-vx1)-100,vt_channel_height).data(),
// 							SString::toFormat("%s %s %s",!bLeftToRight?desc:pChn->int_chn_desc,bLeftToRight?"<--":"-->",bLeftToRight?desc:pChn->int_chn_desc).data());

						vy += vt_channel_height;
						pChn = p->pSmvIn->m_Channel.FetchNext(pos3);
					}
					AddLine(&SvgXml,vx1+50,vy-vt_channel_height,vx2-50,vy-vt_channel_height,false,SString::toFormat("z=100;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;"/*,sColor.data()*/).data());
				}
				else if(p->type == 2 || p->type == 4)
				{
					stuSclVtIedGooseInChannel *pChn = p->pGooseIn->m_Channel.FetchFirst(pos3);
					while(pChn)
					{
						sColor = GetCBLineColor(p->pGooseOut,pChn);
						if(sColor != "#FFFFFF" && sColor !="#55FF00")
						{
							// 							if(sColor == "#55FF00")
							// 								sColor = "#061400";
							SvgXml.AddChildNode("rect",SString::toFormat("uri=;caption=;x=%f;y=%f;z=101;width=%f;height=13;stroke=%s;stroke-width=1;stroke-dasharray=0;fill=%s;",
								vx1+51,vy-vt_channel_height+1,vx2-vx1-102,sColor.data(),sColor.data()).data());
						}
						AddLine(&SvgXml,vx1+50,vy-vt_channel_height,vx2-50,vy-vt_channel_height,false,SString::toFormat("z=100;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;"/*,sColor.data()*/).data());

						const char* desc = NULL;
						pFcda = pChn->m_pFcda;
						if(pFcda != NULL)
						{
							desc = pFcda->dai_desc;
							if(desc == NULL || desc[0] == '\0')
								desc = pFcda->doi_desc;
							if(desc == NULL || desc[0] == '\0')
								desc = pFcda->ln_desc;
						}
						if(desc == NULL)
							desc = "";
						SString fcda_desc,chn_desc;
						if(pFcda == NULL)
							fcda_desc = "【尚未连接】";
						else
							fcda_desc.sprintf("%2d:%s",pFcda->chn_no,desc);
						if(fcda_desc.length() > 22)
						{
							fcda_desc = fcda_desc.left(22);
							char *pStr = fcda_desc.data();
							bool bChn=false;
							while(*pStr != '\0')
							{
								if(bChn)
									bChn = false;
								else if((BYTE)*pStr >= 0x80)
									bChn = true;
								pStr++;
							}
							if(bChn)
								pStr[-1] = '\0';
							fcda_desc = fcda_desc+" ...";
						}
						chn_desc.sprintf("%2d:%s",pChn->int_chn_no,pChn->int_chn_desc);
						if(chn_desc.length() > 22)
						{
							chn_desc = chn_desc.left(22);
							char *pStr = chn_desc.data();
							bool bChn=false;
							while(*pStr != '\0')
							{
								if(bChn)
									bChn = false;
								else if((BYTE)*pStr >= 0x80)
									bChn = true;
								pStr++;
							}
							if(bChn)
								pStr[-1] = '\0';
							chn_desc = chn_desc+" ...";
							//chn_desc = chn_desc.left(22)+" ...";
						}
						SvgXml.AddChildNode("recttext",SString::toFormat("uri=%s;caption=;x=%f;y=%f;z=110;width=%d;height=%d;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
							p->type==2?SString::toFormat("gochin.%s$%d$%s$%d",pSvgIed->pRelaIed->name,p->pGooseOut->gse_no,pIed->name,pChn->int_chn_no).data():
							SString::toFormat("gochin.%s$%d$%s$%d",pIed->name,p->pGooseOut->gse_no,pSvgIed->pRelaIed->name,pChn->int_chn_no).data(),
							(vx1+vx2)/2-145,vy-vt_channel_height,290,vt_channel_height).data(),
							SString::toFormat("%s",bLeftToRight?"<--":"-->").data());
						SvgXml.AddChildNode("recttext",SString::toFormat("uri=;caption=;x=%f;y=%f;z=110;width=%f;height=%d;alignv=center;alignh=right;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
							vx1,vy-vt_channel_height,(vx1+vx2)/2-(vx1)-15,vt_channel_height).data(),
							SString::toFormat("%s",!bLeftToRight?fcda_desc.data():chn_desc.data()).data());
						SvgXml.AddChildNode("recttext",SString::toFormat("uri=;caption=;x=%f;y=%f;z=110;width=%f;height=%d;alignv=center;alignh=left;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
							(vx1+vx2)/2+15,vy-vt_channel_height,(vx2)-(vx1+vx2)/2-20,vt_channel_height).data(),
							SString::toFormat("%s",bLeftToRight?fcda_desc.data():chn_desc.data()).data());

// 						SvgXml.AddChildNode("recttext",SString::toFormat("uri=;caption=;x=%f;y=%f;z=110;width=%f;height=%d;alignv=center;alignh=center;font-family=宋体;font-size=11;font-weight=normal;fill=#FFFFFF;",
// 							vx1+50,vy-vt_channel_height,vx2-vx1-100,vt_channel_height).data(),
// 							SString::toFormat("%s %s %s",!bLeftToRight?desc:pChn->int_chn_desc,bLeftToRight?"<--":"-->",bLeftToRight?desc:pChn->int_chn_desc).data());

						vy += vt_channel_height;
						pChn = p->pGooseIn->m_Channel.FetchNext(pos3);
					}
					AddLine(&SvgXml,vx1+50,vy-vt_channel_height,vx2-50,vy-vt_channel_height,false,SString::toFormat("z=100;stroke=#FFFFFF;stroke-width=1;stroke-dasharray=0;"/*,sColor.data()*/).data());
				}

				//y += h/2;
				y0 += h/2;
				y0 += vt_channel_height;

				p = pSvgIed->items.FetchNext(pos2);
			}
// 			if(pSvgIed->ied_height < 65+ap_height*cb_cnt)
// 				pSvgIed->ied_height = 65+ap_height*cb_cnt;
// 			all_ied_height += pSvgIed->ied_height;
			y += pSvgIed->ied_height;
			pIed2BodyRect->SetAttributeI("height",pSvgIed->ied_height);
			pSvgIed = m_RelaIeds.FetchNext(pos1);
			if(pSvgIed != NULL && i == 0 && pSvgIed->items[0]->type != 1 && y>= all_ied_height/2)
				break;
			y+=5;//IED间Y方向间隔
		}
		if(pIedBodyRect->GetAttributeF("height") < y + 50)
			pIedBodyRect->SetAttributeI("height",y + 50);

	}

	pWndRect->SetAttributeF("height",pIedBodyRect->GetAttributeF("height")+50);
	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  添加线段
// 作    者:  邵凯田
// 创建时间:  2016-1-12 9:02
// 参数说明:  
// 返 回 值:  
//////////////////////////////////////////////////////////////////////////
void CSpVtSvgGenerator::AddLine(SBaseConfig *pRootNode, int x1,int y1,int x2,int y2,bool bArrow,char* sAttrs)
{
	if(x1 == x2)
	{
		//垂直线
		//x1 = x2 = NewVLineX(x1);
		AddBeeLine(pRootNode,x1,y1,x2,y2,bArrow,sAttrs);
	}
	else if(y1 == y2)
	{
		//水平线
		//y1 = y2 = NewHLineY(y1);
		AddBeeLine(pRootNode,x1,y1,x2,y2,bArrow,sAttrs);
	}
	else
	{
		AddBeeLine(pRootNode,x1,y1,x2,y2,bArrow,sAttrs);

// 		//水平为主的拆线段，台阶状
// 		int cx = (x1+x2)/2;//水平中心
// 		y1 = NewHLineY(y1);
// 		cx = NewVLineX(cx);
// 		y2 = NewHLineY(y2);
// 		AddBeeLine(pRootNode,x1,y1,cx,y1,false,sAttrs);
// 		AddBeeLine(pRootNode,cx,y1,cx,y2,false,sAttrs);
// 		AddBeeLine(pRootNode,cx,y2,x2,y2,bArrow,sAttrs);
	}
}

void CSpVtSvgGenerator::AddBeeLine(SBaseConfig *pRootNode, int x1,int y1,int x2,int y2,bool bArrow,char* sAttrs)
{
	SBaseConfig *pLineNode = pRootNode->AddChildNode("line",sAttrs);
	pLineNode->SetAttributeI("x1",x1);
	pLineNode->SetAttributeI("y1",y1);
	pLineNode->SetAttributeI("x2",x2);
	pLineNode->SetAttributeI("y2",y2);
	if(bArrow)
	{
		//以末端为中心点，创建箭头并旋转至合适角度
		float cx = x2;
		float cy = y2;
		float rx = x1-x2;
		float ry = y1-y2;
		float dis = sqrt(rx*rx+ry*ry);
		if(dis<0.01)
			return;
		bool bConv=false;
		if(y2>y1)
			bConv = true;
		float fcos = (bConv?-rx:rx)/dis;
		float radian = acos(fcos)-3.1415926;//0~2PI
 		if(bConv)
		{
			radian += 3.1415926;
		}
		float x3 = (cx-10);
		float y3 = cy-3;
		float x4 = (cx-10);
		float y4 = cy+3;

		float x30 = (float)(x3-cx)*cos(radian)-(y3-cy)*sin(radian)+cx;
		float x40 = (float)(x4-cx)*cos(radian)-(y4-cy)*sin(radian)+cx;
		float y30 = (float)(x3-cx)*sin(radian)+(y3-cy)*cos(radian)+cy;
		float y40 = (float)(x4-cx)*sin(radian)+(y4-cy)*cos(radian)+cy;
		
		SString sAttr;
		sAttr.sprintf("points=%f,%f %f,%f %f,%f %f,%f;%s",cx,cy,x30,y30,x40,y40,cx,cy,sAttrs);
		pLineNode = pRootNode->AddChildNode("polyline",sAttr.data());
		pLineNode->SetAttribute("fill",pLineNode->GetAttribute("stroke"));
		pLineNode->SetAttribute("stroke-dasharray","0");
	}
}

CSpVtSvgGenerator::stuSvgVtCbIed* CSpVtSvgGenerator::SearchRelaIed(stuSclVtIed *pRelaIed)
{
	unsigned long pos=0;
	stuSvgVtCbIed *p = m_RelaIeds.FetchFirst(pos);
	while(p)
	{
		if(p->pRelaIed == pRelaIed)
			return p;
		p = m_RelaIeds.FetchNext(pos);
	}
	p = new stuSvgVtCbIed;
	p->pRelaIed = pRelaIed;
	m_RelaIeds.append(p);
	return p;
}

void CSpVtSvgGenerator::SortRelaIed()
{
	SPtrList<stuSvgVtCbIed> tmp;
	m_RelaIeds.setAutoDelete(false);
	stuSvgVtCbIed *p,*p2;
	unsigned long pos1,pos2;
	//将所有SMV输入的排序
	while(1)
	{
		p2 = NULL;
		p = m_RelaIeds.FetchFirst(pos1);
		while(p)
		{
			stuSvgVtCbIedItem *pItem = p->items.FetchFirst(pos2);
			while(pItem)
			{
				if(pItem->type == 1)
					break;
				pItem = p->items.FetchNext(pos2);
			}
			if(pItem != NULL)
			{
				//有SMV-IN
				if(p2 == NULL)
					p2 = p;
				else if(strcmp(p->pRelaIed->name,p2->pRelaIed->name)<0)
					p2 = p;
			}
			p = m_RelaIeds.FetchNext(pos1);
		}
		if(p2 == NULL)
			break;
		tmp.append(p2);
		m_RelaIeds.remove(p2);
	}

	//对余下所有GOOSE输入的排序
	while(1)
	{
		p2 = NULL;
		p = m_RelaIeds.FetchFirst(pos1);
		while(p)
		{
			stuSvgVtCbIedItem *pItem = p->items.FetchFirst(pos2);
			while(pItem)
			{
				if(pItem->type == 2)
					break;
				pItem = p->items.FetchNext(pos2);
			}
			if(pItem != NULL)
			{
				//有GSE-IN
				if(p2 == NULL)
					p2 = p;
				else if(strcmp(p->pRelaIed->name,p2->pRelaIed->name)<0)
					p2 = p;
			}
			p = m_RelaIeds.FetchNext(pos1);
		}
		if(p2 == NULL)
			break;
		tmp.append(p2);
		m_RelaIeds.remove(p2);
	}

	//对余下所有排序
	while(1)
	{
		p2 = NULL;
		p = m_RelaIeds.FetchFirst(pos1);
		while(p)
		{				
			if(p2 == NULL)
				p2 = p;
			else if(strcmp(p->pRelaIed->name,p2->pRelaIed->name)<0)
				p2 = p;
			p = m_RelaIeds.FetchNext(pos1);
		}
		if(p2 == NULL)
			break;
		tmp.append(p2);
		m_RelaIeds.remove(p2);
	}

	//排序后还入实例队列
	tmp.copyto(m_RelaIeds);
	m_RelaIeds.setAutoDelete(true);
}

SString CSpVtSvgGenerator::GetCBColor(stuSclVtIedSmvOut *pOut)
{
	if(pOut == NULL || pOut->m_pDataset==NULL)
		return "#FF0000";
	else if(pOut->bActive == false)
		return "#353535";//静态配置
	else if(pOut->bInvalid)
		return "#FF0000";
	else if(pOut->bTest)
		return "#AA7700";
	else
		return "#295300";
}

SString CSpVtSvgGenerator::GetCBColor(stuSclVtIedGooseOut *pOut)
{
	if(pOut == NULL || pOut->m_pDataset==NULL)
		return "#FF0000";
	else if(pOut->bActive == false)
		return "#353535";//静态配置
	//else if(pOut->bInvalid)
	//	return "#FF0000";
	else if(pOut->bTest)
		return "#AA7700";
	else
		return "#FFFFFF";
}

SString CSpVtSvgGenerator::GetCBLineColor(stuSclVtIedSmvOut *pOut,stuSclVtIedSmvInChannel *pChn/*=NULL*/)
{
	//return "#AA7700";
	if(pOut == NULL || pOut->m_pDataset==NULL || (pChn!= NULL && pChn->m_pFcda == NULL))
		return "#FF0000";
	else if(pOut->bActive == false)
		return "#FFFFFF";//静态配置
	else if(pOut->bInvalid)
		return "#FF0000";
	else if(pOut->bTest || (pChn!= NULL && pChn->m_pFcda->IsTest()))
		return "#AA7700";
	else
		return "#55FF00";
}

SString CSpVtSvgGenerator::GetCBLineColor(stuSclVtIedGooseOut *pOut,stuSclVtIedGooseInChannel *pChn/*=NULL*/)
{
	//return "#FF0000";
	if(pOut == NULL || pOut->m_pDataset==NULL || (pChn!= NULL && pChn->m_pFcda == NULL))
		return "#FF0000";
	else if(pOut->bActive == false)
		return "#FFFFFF";//静态配置
	// 		else if(pOut->bInvalid)
	// 			return "#FF0000";
	else if(pOut->bTest)
		return "#AA7700";
	else
		return "#55FF00";
}

CSpVtSvgAccessPoint::CSpVtSvgAccessPoint(QWidget *parent) : SSvgWnd(parent)
{
	m_pGenerator = NULL;
}

CSpVtSvgAccessPoint::~CSpVtSvgAccessPoint()
{

}

SString CSpVtSvgAccessPoint::GetSvgObjectTipText(SSvgObject *pSvgObj)
{
	if(m_pGenerator == NULL || m_pGenerator->m_pSclVt == NULL)
		return SSvgWnd::GetSvgObjectTipText(pSvgObj);
	SString uri = pSvgObj->GetAttribute("uri");
	if(uri == "home")
		return "返回首页";
	else if(uri.left(7) == "subnet.")
	{
		SString sSubNetName = uri.mid(7);
		return sSubNetName;
	}
	else if(uri.left(4) == "ied.")
	{
		SString sIedName = uri.mid(4);
		stuSclVtIed *pIed = NULL;
		if(m_pGenerator != NULL && m_pGenerator->m_pSclVt != NULL)
			pIed = m_pGenerator->m_pSclVt->SearchIedByName(sIedName.data());
		if(pIed != NULL)
			sIedName.sprintf("[%s]\n%s\n制造商:%s 型号:%s",
			pIed->name,pIed->desc,pIed->manufacturer,pIed->type);
		return sIedName;
	}
	else if(uri.left(5) == "svcb.")
	{
		SString sTemp = uri.mid(5);
		SString ied_name = SString::GetIdAttribute(1,sTemp,"$");
		int appid = SString::GetIdAttribute(2,sTemp,"$").toInt();
		stuSclVtIed *pIed = NULL;
		if(m_pGenerator != NULL && m_pGenerator->m_pSclVt != NULL)
			pIed = m_pGenerator->m_pSclVt->SearchIedByName(ied_name.data());
		stuSclVtIedSmvOut *pSmvOut = NULL;
		if(pIed != NULL)
		{
			pSmvOut = pIed->SearchSmvOutByAppid(appid);
		}
		if(pSmvOut != NULL)
		{
			sTemp.sprintf("%s[%s] [SV控制块:共%d项]\n",pIed->name,pIed->desc,pSmvOut->m_pDataset==NULL?0:pSmvOut->m_pDataset->m_Fcda.count());
			sTemp += SString::toFormat("名称:%s  数据集:%s  配置版本:%d VLAN:%d \nsvID:%s AppId:0x%04X MAC:%s\n",
				pSmvOut->name,pSmvOut->ds_name.data(),pSmvOut->confRev,pSmvOut->vlan,pSmvOut->smvID,pSmvOut->appid,pSmvOut->mac);
			if(pSmvOut->m_pDataset != NULL)
			{
				unsigned long pos=0;
				stuSclVtIedDatasetFcda *pFcda = pSmvOut->m_pDataset->m_Fcda.FetchFirst(pos);
				int cnt=0;
				while(pFcda)
				{
					if(++cnt > 25)
					{
						sTemp += SString::toFormat("省略%d项...",pSmvOut->m_pDataset->m_Fcda.count()-25);
						break;
					}
					const char* desc = pFcda->dai_desc;
					if(desc == NULL || desc[0] == '\0')
						desc = pFcda->doi_desc;
					if(desc == NULL || desc[0] == '\0')
						desc = pFcda->ln_desc;
					if(desc == NULL)
						desc = "";
					SString val = pFcda->GetValue();
					if(val.length() == 0)
						sTemp += SString::toFormat("%2d:%s\n",
							pFcda->chn_no,desc);
					else
						sTemp += SString::toFormat("%2d:%s(%s)\n",
						pFcda->chn_no,desc,val.data());

					pFcda = pSmvOut->m_pDataset->m_Fcda.FetchNext(pos);
					if(pFcda == NULL)
						((char*)sTemp.data())[sTemp.length()-1] = '\0';
				}
			}
		}
		return sTemp;
	}	
	else if(uri.left(5) == "gocb.")
	{
		SString sTemp = uri.mid(5);
		SString ied_name = SString::GetIdAttribute(1,sTemp,"$");
		int appid = SString::GetIdAttribute(2,sTemp,"$").toInt();
		stuSclVtIed *pIed = NULL;
		if(m_pGenerator != NULL && m_pGenerator->m_pSclVt != NULL)
			pIed = m_pGenerator->m_pSclVt->SearchIedByName(ied_name.data());
		stuSclVtIedGooseOut *pGooseOut = NULL;
		if(pIed != NULL)
		{
			pGooseOut = pIed->SearchGooseOutByAppid(appid);
		}
		if(pGooseOut != NULL)
		{
			sTemp.sprintf("%s[%s] [GOOSE控制块:共%d项]\n",pIed->name,pIed->desc,pGooseOut->m_pDataset==NULL?0:pGooseOut->m_pDataset->m_Fcda.count());
			sTemp += SString::toFormat("名称:%s  数据集:%s  配置版本:%d VLAN:%d \ngoID:%s AppId:0x%04X MAC:%s\n",
				pGooseOut->name,pGooseOut->ds_name.data(),pGooseOut->confRev,pGooseOut->vlan,pGooseOut->gocb,pGooseOut->appid,pGooseOut->mac);
			if(pGooseOut->m_pDataset != NULL)
			{
				unsigned long pos=0;
				stuSclVtIedDatasetFcda *pFcda = pGooseOut->m_pDataset->m_Fcda.FetchFirst(pos);
				int cnt = 0;
				while(pFcda)
				{
					if(++cnt > 25)
					{
						sTemp += SString::toFormat("省略%d项...",pGooseOut->m_pDataset->m_Fcda.count()-25);
						break;
					}
					const char* desc = pFcda->dai_desc;
					if(desc == NULL || desc[0] == '\0')
						desc = pFcda->doi_desc;
					if(desc == NULL || desc[0] == '\0')
						desc = pFcda->ln_desc;
					if(desc == NULL)
						desc = "";
					SString val = pFcda->GetValue();
					if(val.length() == 0)
						sTemp += SString::toFormat("%2d:%s\n",
						pFcda->chn_no,desc);
					else
						sTemp += SString::toFormat("%2d:%s(%s)\n",
						pFcda->chn_no,desc,val.data());
					pFcda = pGooseOut->m_pDataset->m_Fcda.FetchNext(pos);
					if(pFcda == NULL)
						((char*)sTemp.data())[sTemp.length()-1] = '\0';
				}
			}
		}
		return sTemp;
	}
	else if(uri.left(5) == "svin.")
	{
		SString sTemp = uri.mid(5);
		SString out_ied_name = SString::GetIdAttribute(1,sTemp,"$");
		int out_smv_no = SString::GetIdAttributeI(2,sTemp,"$");
		SString in_ied_name = SString::GetIdAttribute(3,sTemp,"$");
		stuSclVtIed *pOutIed = m_pGenerator->m_pSclVt->SearchIedByName(out_ied_name.data());
		stuSclVtIed *pInIed = m_pGenerator->m_pSclVt->SearchIedByName(in_ied_name.data());
		if(pInIed == NULL)
			return sTemp;
		stuSclVtIedSmvIn *pSmvIn = pInIed->SearchSmvInByExtNo(out_ied_name.data(),out_smv_no);
		if(pSmvIn == NULL)
			return sTemp;
		
		sTemp.sprintf("订阅:[%s] %s [SV输入虚端子:共%d项]\n",pInIed->name,pInIed->desc,pSmvIn->m_Channel.count());
		if(pSmvIn->m_pSmvOut == NULL)
			sTemp += "尚未连接输入端子!\n";
		else
			sTemp += SString::toFormat("发布:[%s] %s  数据集:%s\n     AppId:0x%04X VLAN:%d MAC:%s\n",
				pOutIed->name,pOutIed->desc,pSmvIn->m_pSmvOut->ds_name.data(),pSmvIn->m_pSmvOut->appid,pSmvIn->m_pSmvOut->vlan,pSmvIn->m_pSmvOut->mac);
		unsigned long pos=0;
		int cnt = 0;
		stuSclVtIedSmvInChannel *pChn = pSmvIn->m_Channel.FetchFirst(pos);
		int max_in_desc_len=10;
		while(pChn)
		{
			if((int)strlen(pChn->int_chn_desc) > max_in_desc_len)
				max_in_desc_len = strlen(pChn->int_chn_desc);
			pChn = pSmvIn->m_Channel.FetchNext(pos);
		}
		stuSclVtIedDatasetFcda *pFcda;
		sTemp += SString::toFormat("\n   %s        【发布侧】\n",
			SString::toFillString("【订阅侧】",max_in_desc_len,' ',false).data());
		pChn = pSmvIn->m_Channel.FetchFirst(pos);
		while(pChn)
		{
			if(++cnt > 25)
			{
				sTemp += SString::toFormat("省略%d项...",pSmvIn->m_Channel.count()-25);
				break;
			}
			pFcda = pChn->m_pFcda;
			if(pFcda == NULL)
			{
				sTemp += SString::toFormat("%2d:%s  【尚未连接】\n",
					pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data());
			}
			else
			{
				const char* desc = pFcda->dai_desc;
				if(desc == NULL || desc[0] == '\0')
					desc = pFcda->doi_desc;
				if(desc == NULL || desc[0] == '\0')
					desc = pFcda->ln_desc;
				if(desc == NULL)
					desc = "";
				SString val = pFcda->GetValue();
				if(val.length() == 0)
					sTemp += SString::toFormat("%2d:%s <-- %2d:%s\n",
						pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data(),pFcda->chn_no,desc);
				else
					if(val.length() == 0)
						sTemp += SString::toFormat("%2d:%s <-- %2d:%s(%s)\n",
						pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data(),pFcda->chn_no,desc,val.data());
			}
			pChn = pSmvIn->m_Channel.FetchNext(pos);
			if(pChn == NULL)
				((char*)sTemp.data())[sTemp.length()-1] = '\0';
		}
		return sTemp;
	}
	else if(uri.left(5) == "goin.")
	{
		SString sTemp = uri.mid(5);
		SString out_ied_name = SString::GetIdAttribute(1,sTemp,"$");
		int out_gse_no = SString::GetIdAttributeI(2,sTemp,"$");
		SString in_ied_name = SString::GetIdAttribute(3,sTemp,"$");
		stuSclVtIed *pOutIed = m_pGenerator->m_pSclVt->SearchIedByName(out_ied_name.data());
		stuSclVtIed *pInIed = m_pGenerator->m_pSclVt->SearchIedByName(in_ied_name.data());
		if(pInIed == NULL)
			return sTemp;
		stuSclVtIedGooseIn *pGooseIn = pInIed->SearchGooseInByExtNo(out_ied_name.data(),out_gse_no);
		if(pGooseIn == NULL)
			return sTemp;

		sTemp.sprintf("订阅:[%s] %s [GOOSE输入虚端子:共%d项]\n",pInIed->name,pInIed->desc,pGooseIn->m_Channel.count());
		if(pGooseIn->m_pGooseOut == NULL)
			sTemp += "尚未连接输入端子!\n";
		else
			sTemp += SString::toFormat("发布:[%s] %s  数据集:%s\n     AppId:0x%04X VLAN:%d MAC:%s\n",
			pOutIed->name,pOutIed->desc,pGooseIn->m_pGooseOut->ds_name.data(),pGooseIn->m_pGooseOut->appid,pGooseIn->m_pGooseOut->vlan,pGooseIn->m_pGooseOut->mac);
		unsigned long pos=0;
		int cnt = 0;
		stuSclVtIedGooseInChannel *pChn = pGooseIn->m_Channel.FetchFirst(pos);
		int max_in_desc_len=10;
		while(pChn)
		{
			if((int)strlen(pChn->int_chn_desc) > max_in_desc_len)
				max_in_desc_len = strlen(pChn->int_chn_desc);
			pChn = pGooseIn->m_Channel.FetchNext(pos);
		}
		stuSclVtIedDatasetFcda *pFcda;
		sTemp += SString::toFormat("\n   %s        【发布侧】\n",
			SString::toFillString("【订阅侧】",max_in_desc_len,' ',false).data());

		pChn = pGooseIn->m_Channel.FetchFirst(pos);
		while(pChn)
		{
			if(++cnt > 25)
			{
				sTemp += SString::toFormat("省略%d项...",pGooseIn->m_Channel.count()-25);
				break;
			}
			pFcda = pChn->m_pFcda;
			if(pFcda == NULL)
			{
				sTemp += SString::toFormat("%2d:%s  【尚未连接】\n",
					pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data());
			}
			else
			{
				const char* desc = pFcda->dai_desc;
				if(desc == NULL || desc[0] == '\0')
					desc = pFcda->doi_desc;
				if(desc == NULL || desc[0] == '\0')
					desc = pFcda->ln_desc;
				if(desc == NULL)
					desc = "";
				SString val = pFcda->GetValue();
				if(val.length() == 0)
					sTemp += SString::toFormat("%2d:%s <-- %2d:%s\n",
						pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data(),pFcda->chn_no,desc);
				else
					sTemp += SString::toFormat("%2d:%s <-- %2d:%s(%s)\n",
					pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data(),pFcda->chn_no,desc,val.data());
			}
			pChn = pGooseIn->m_Channel.FetchNext(pos);
			if(pChn == NULL)
				((char*)sTemp.data())[sTemp.length()-1] = '\0';
		}
		return sTemp;
	}
	else if(uri.left(7) == "svchin.")
	{
		SString sTemp = uri.mid(7);
		SString out_ied_name = SString::GetIdAttribute(1,sTemp,"$");
		int out_smv_no = SString::GetIdAttributeI(2,sTemp,"$");
		SString in_ied_name = SString::GetIdAttribute(3,sTemp,"$");
		int in_no = SString::GetIdAttributeI(4,sTemp,"$");
		stuSclVtIed *pOutIed = m_pGenerator->m_pSclVt->SearchIedByName(out_ied_name.data());
		stuSclVtIed *pInIed = m_pGenerator->m_pSclVt->SearchIedByName(in_ied_name.data());
		if(pInIed == NULL)
			return sTemp;
		stuSclVtIedSmvIn *pSmvIn = pInIed->SearchSmvInByExtNo(out_ied_name.data(),out_smv_no);
		if(pSmvIn == NULL)
			return sTemp;

		sTemp.sprintf("订阅:[%s] %s [SV输入虚端子:共%d项]\n",pInIed->name,pInIed->desc,pSmvIn->m_Channel.count());
		if(pSmvIn->m_pSmvOut == NULL)
			sTemp += "尚未连接输入端子!\n";
		else
			sTemp += SString::toFormat("发布:[%s] %s  数据集:%s\n     AppId:0x%04X VLAN:%d MAC:%s\n",
			pOutIed->name,pOutIed->desc,pSmvIn->m_pSmvOut->ds_name.data(),pSmvIn->m_pSmvOut->appid,pSmvIn->m_pSmvOut->vlan,pSmvIn->m_pSmvOut->mac);
		unsigned long pos=0;
		int cnt = 0;
		stuSclVtIedSmvInChannel *pChn = pSmvIn->m_Channel.FetchFirst(pos);
		int max_in_desc_len=10;
		while(pChn)
		{
			if((int)strlen(pChn->int_chn_desc) > max_in_desc_len)
				max_in_desc_len = strlen(pChn->int_chn_desc);
			pChn = pSmvIn->m_Channel.FetchNext(pos);
		}
		stuSclVtIedDatasetFcda *pFcda;
		sTemp += SString::toFormat("\n   %s        【发布侧】\n",
			SString::toFillString("【订阅侧】",max_in_desc_len,' ',false).data());
		pChn = pSmvIn->m_Channel.FetchFirst(pos);
		while(pChn)
		{
			if(pChn->int_chn_no == in_no)
			{
				if(++cnt > 25)
				{
					sTemp += SString::toFormat("省略%d项...",pSmvIn->m_Channel.count()-25);
					break;
				}
				pFcda = pChn->m_pFcda;
				if(pFcda == NULL)
				{
					sTemp += SString::toFormat("%2d:%s  【尚未连接】\n",
						pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data());
				}
				else
				{
					const char* desc = pFcda->dai_desc;
					if(desc == NULL || desc[0] == '\0')
						desc = pFcda->doi_desc;
					if(desc == NULL || desc[0] == '\0')
						desc = pFcda->ln_desc;
					if(desc == NULL)
						desc = "";
					SString val = pFcda->GetValue();
					if(val.length() == 0)
						sTemp += SString::toFormat("%2d:%s <-- %2d:%s\n",
						pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data(),pFcda->chn_no,desc);
					else
						sTemp += SString::toFormat("%2d:%s <-- %2d:%s(%s)\n",
						pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data(),pFcda->chn_no,desc,val.data());
				}
				((char*)sTemp.data())[sTemp.length()-1] = '\0';
				break;
			}
			pChn = pSmvIn->m_Channel.FetchNext(pos);
		}
		return sTemp;
	}
	else if(uri.left(7) == "gochin.")
	{
		SString sTemp = uri.mid(7);
		SString out_ied_name = SString::GetIdAttribute(1,sTemp,"$");
		int out_gse_no = SString::GetIdAttributeI(2,sTemp,"$");
		SString in_ied_name = SString::GetIdAttribute(3,sTemp,"$");
		int in_no = SString::GetIdAttributeI(4,sTemp,"$");
		stuSclVtIed *pOutIed = m_pGenerator->m_pSclVt->SearchIedByName(out_ied_name.data());
		stuSclVtIed *pInIed = m_pGenerator->m_pSclVt->SearchIedByName(in_ied_name.data());
		if(pInIed == NULL)
			return sTemp;
		stuSclVtIedGooseIn *pGooseIn = pInIed->SearchGooseInByExtNo(out_ied_name.data(),out_gse_no);
		if(pGooseIn == NULL)
			return sTemp;

		sTemp.sprintf("订阅:[%s] %s [GOOSE输入虚端子:共%d项]\n",pInIed->name,pInIed->desc,pGooseIn->m_Channel.count());
		if(pGooseIn->m_pGooseOut == NULL)
			sTemp += "尚未连接输入端子!\n";
		else
			sTemp += SString::toFormat("发布:[%s] %s  数据集:%s\n     AppId:0x%04X VLAN:%d MAC:%s\n",
			pOutIed->name,pOutIed->desc,pGooseIn->m_pGooseOut->ds_name.data(),pGooseIn->m_pGooseOut->appid,pGooseIn->m_pGooseOut->vlan,pGooseIn->m_pGooseOut->mac);
		unsigned long pos=0;
		int cnt = 0;
		stuSclVtIedGooseInChannel *pChn = pGooseIn->m_Channel.FetchFirst(pos);
		int max_in_desc_len=10;
		while(pChn)
		{
			if((int)strlen(pChn->int_chn_desc) > max_in_desc_len)
				max_in_desc_len = strlen(pChn->int_chn_desc);
			pChn = pGooseIn->m_Channel.FetchNext(pos);
		}
		stuSclVtIedDatasetFcda *pFcda;
		sTemp += SString::toFormat("\n   %s        【发布侧】\n",
			SString::toFillString("【订阅侧】",max_in_desc_len,' ',false).data());

		pChn = pGooseIn->m_Channel.FetchFirst(pos);
		while(pChn)
		{
			if(pChn->int_chn_no == in_no)
			{
				if(++cnt > 25)
				{
					sTemp += SString::toFormat("省略%d项...",pGooseIn->m_Channel.count()-25);
					break;
				}
				pFcda = pChn->m_pFcda;
				if(pFcda == NULL)
				{
					sTemp += SString::toFormat("%2d:%s  【尚未连接】\n",
						pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data());
				}
				else
				{
					const char* desc = pFcda->dai_desc;
					if(desc == NULL || desc[0] == '\0')
						desc = pFcda->doi_desc;
					if(desc == NULL || desc[0] == '\0')
						desc = pFcda->ln_desc;
					if(desc == NULL)
						desc = "";
					SString val = pFcda->GetValue();
					if(val.length() == 0)
						sTemp += SString::toFormat("%2d:%s <-- %2d:%s\n",
							pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data(),pFcda->chn_no,desc);
					else
						sTemp += SString::toFormat("%2d:%s <-- %2d:%s(%s)\n",
						pChn->int_chn_no,SString::toFillString(pChn->int_chn_desc,max_in_desc_len,' ',false).data(),pFcda->chn_no,desc,val.data());
				}
				((char*)sTemp.data())[sTemp.length()-1] = '\0';
				break;
			}
			pChn = pGooseIn->m_Channel.FetchNext(pos);
		}
		return sTemp;
	}
	else if(uri == "v_line")
		return "切换到虚链路图";
	else if(uri == "v_terminal")
		return "切换到虚端子图";

	return pSvgObj->GetAttribute("caption");
}

void CSpVtSvgAccessPoint::OnClickObject(SSvgObject *pSvgObj)
{
	SString uri = pSvgObj->GetAttribute("uri"); 
	if(uri.left(7) == "subnet.")
	{
		SString sSubNetName = uri.mid(7);
		GotoSubNet(sSubNetName);
	}
	else if(uri.left(4) == "ied.")
	{
		SString sIedName = uri.mid(4);
		GotoIed(sIedName);
	}
	else if(uri.left(4) == "home")
	{
		GotoSubNet(m_sLastSubNetName);
	}
	else if(uri == "v_line")
	{
		if(m_pGenerator != NULL)
			m_pGenerator->m_bShowVT = false;
		GotoIed(m_sLastIedName);
	}
	else if(uri == "v_terminal")
	{
		if(m_pGenerator != NULL)
			m_pGenerator->m_bShowVT = true;
		GotoIed(m_sLastIedName);
	}

}

void CSpVtSvgAccessPoint::GotoSubNet(SString sSubNetName)
{
	SXmlConfig xml;
	m_pGenerator->GeneratorSubNetSvg(sSubNetName,xml);
	Load(&xml);
	if(m_sLastShowType != "subnet")
	{
		ToTopCenter();
	}
	else
	{
		Render();
		RedrawWindow();
	}
	m_sLastShowType = "subnet";
	m_sLastSubNetName = sSubNetName;
}

void CSpVtSvgAccessPoint::GotoIed(SString sIedName)
{
	SXmlConfig xml;
	if(m_pGenerator->m_bShowVT)
		m_pGenerator->GeneratorIedVirtualTerminal(sIedName,xml);
	else
		m_pGenerator->GeneratorIedCBInOut(sIedName,xml);
	Load(&xml);
	//xml.SaveConfig("c:\\vt.svg");
	if(m_sLastShowType != "ied" || sIedName != m_sLastIedName)
	{
		ToTopCenter();
	}
	else
	{
		Render();
		RedrawWindow();
	}
	m_sLastShowType = "ied";
	m_sLastIedName = sIedName;
}