#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(11300)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(5500)},  //XA_GameOver
	//WEEK1A.XA
	{XA_Week1A, XA_LENGTH(12800)}, //XA_Bopeebo
	{XA_Week1A, XA_LENGTH(1800)}, //XA_Fresh
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\WEEK1A.XA;1", //XA_Week1A
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	//WEEK1A.XA
	{"bopeebo", false}, //XA_Bopeebo
	{"fresh", false}, //XA_Bopeebo
	{NULL, false}
};
