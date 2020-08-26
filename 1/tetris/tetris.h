#define POSX	30
#define POSY	1
#define NEXTX	4
#define NEXTY	8
#define INFOX	2
#define INFOY	2
#define STATX	2
#define STATY	17

#define SPEED 	'-'	/* Tastendefinition */
#define RECHTS	'.'
#define LINKS	','
#define DREHEN	' '
#define PAUSE	'p'
#define QUIT	'x'
#define FUSCHEN '!'
#define LFUSCHEN '"'
#define HIGHSCORE 'h'

#define LEER_ZEICHEN    ". "
#define VOLL_ZEICHEN	"[]"

#define ZEIT		100	/* Zeitsteuerung */
#define ZEIT_SCHRITT	10

typedef struct {
	int	nr;
	int	pos;
	int	farbe;
	int	x;
	int	y;
	} t_figur;

typedef struct {
	char	name[21];
	char	datum[16];	/* ]10.10.95, 18:14[ 14 stellen */
	int	lines;
	int	level;
	int	points;
	} t_highscore;

typedef struct {
	int	start_level;
	int	level;
	int	lines;
	int	points;
	} t_score;
