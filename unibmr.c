

//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
#include <stdio.h>
#define PATH_MAX 2500
#if defined(__linux__) //linux
#define MYOS 1
#elif defined(_WIN32)
#define MYOS 2
#elif defined(_WIN64)
#define MYOS 3
#elif defined(__unix__) 
#define MYOS 4  // freebsd
#define PATH_MAX 2500
#else
#define MYOS 0
#endif
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <time.h>

#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"






char *fbasenoext(char* mystr)
{
    char *retstr;
    char *lastdot;
    if (mystr == NULL)
         return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
        return NULL;
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
    return retstr;
}






/// CORE
char targettexfile[PATH_MAX];
char linedata[500][PATH_MAX];
int  lineid    = 1; 
int  lineidmax = 1; 
void printframe( )
{
   FILE *target; 
   int ch ; int foundfig = 0; 

   for( ch = 1 ;  ch <= lineidmax ; ch++ ) 
   {
     if ( linedata[ ch ][0] == '}' ) 
      if ( linedata[ ch ][1] == '{' ) 
         //if ( strstr( linedata[ ch ] , "}[]" ) != 0 ) 
            foundfig = 1;
   }


   target = fopen( targettexfile , "ab+");
   fputs( "\%Frame\n" , target ) ; 
   if ( foundfig == 1 ) fputs( "\%With fig.\n" , target ) ; 
   if ( foundfig == 0 ) fputs( "\%Without fig.\n" , target ) ; 
   for( ch = 1 ;   ch <= lineidmax ; ch++ ) 
   {
     if ( foundfig == 0 ) 
     {
        if ( strstr( linedata[ ch ], "fitimage{" ) != 0 ) 
        {
           fputs( "\%fitimage found{\n" ,target ) ; 
           //fputs( linedata[ ch ], target ) ; 
        }
        else 
        {
           fputs( linedata[ ch ], target ) ; 
        }
     }
     else if ( foundfig == 1 ) 
         fputs( linedata[ ch ], target ) ; 

   }
   fclose( target ); 
   lineid    = 1; 
   lineidmax = 1; 
}





void add2frame( char *myline )
{
   strncpy( linedata[ lineid ], myline ,  PATH_MAX );
   lineidmax = lineid;
   lineid++;
}

void printend( )
{
   FILE *target;
   target = fopen( targettexfile , "ab+");
   fputs( "\%End\n" , target ) ; 
   fputs( "\\end{document}\n" , target ); 
   fclose( target ); 
}


char *fbasename(char *name)
{
  char *base = name;
  while (*name)
    {
      if (*name++ == '/')
	{
	  base = name;
	}
    }
  return (base);
}







int fexist(char *a_option)
{
  char dir1[PATH_MAX]; 
  char *dir2;
  DIR *dip;
  strncpy( dir1 , "",  PATH_MAX  );
  strncpy( dir1 , a_option,  PATH_MAX  );

  struct stat st_buf; 
  int status; 
  int fileordir = 0 ; 

  status = stat ( dir1 , &st_buf);
  if (status != 0) {
    fileordir = 0;
  }

  // this is compatible to check if a file exists
  FILE *fp2check = fopen( dir1  ,"r");
  if( fp2check ) {
  // exists
  fileordir = 1; 
  fclose(fp2check);
  } 

  if (S_ISDIR (st_buf.st_mode)) {
    fileordir = 2; 
  }
return fileordir;
/////////////////////////////
}





////////////////////////////////////////////////////////////////////
char *strcut( char *str , int myposstart, int myposend )
{     // copyleft, C function made by Spartrekus 
      char ptr[strlen(str)+1];
      int i,j=0;
      for(i=0; str[i]!='\0'; i++)
      {
        if ( ( str[i] != '\0' ) && ( str[i] != '\0') )
         if ( ( i >=  myposstart-1 ) && (  i <= myposend-1 ) )
           ptr[j++]=str[i];
      } 
      ptr[j]='\0';
      size_t siz = sizeof ptr ; 
      char *r = malloc( sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}





///////////////////////////////////////////
void readfile( char *filesource )
{
   FILE *source; 
   int ch ; 
   source = fopen( filesource , "r");
   while( ( ch = fgetc(source) ) != EOF )
   {
         printf( "%c", ch );
   }
   fclose(source);
}






///////////////////////////////////////////
void includefile( char *fileoutput , char *filesource )
{
   FILE *source; 
   FILE *target; 
   int ch, c, co;
   char lline[PATH_MAX];
   char charo[PATH_MAX];
   int pcc = 0;
   int linecount = 0;
   int posx    = 100;
   int posy    = 730;
   int posxmax = 588;
   int posymax = 835;


   source = fopen( filesource , "r");
   target = fopen( fileoutput , "wb+");
   fputs( "\n" , target );
   fputs( "\n" , target );
   fputs( "%%Converted using the C programming language.\n" , target );
   fputs( "\n" , target );
   fputs( "\n" , target );

   FILE *fp5;
   char ncpfile[PATH_MAX];
   char fileargorg[PATH_MAX];
   char filearg[PATH_MAX];
   char fooline[PATH_MAX];
   char foostr[PATH_MAX];
   int export_skip = 0;
   int area_archive = 0;
   int fileeof = 0;
   int mode_open_frame = 0;
   int mode_frame_level = 0;
   int mode_frame_fig = 0;
   while( fileeof == 0 )
   {
       export_skip = 0;
       c = fgetc(source); 
       if ( c == EOF ) fileeof = 1;
       // EOF

       else
       {

           if ( c != '\n' )
               lline[pcc++]=c;

           if ( ( lline[ 0 ] == '!' )  && ( lline[ 1 ] == 'E' ) && ( lline[ 2 ] == 'O' )  && ( lline[ 3 ] == 'F' )  )
              fileeof = 1;

           if ( ( lline[ 0 ] == '/' )  && ( lline[ 1 ] == '*' ) ) 
               area_archive = 1;

           if ( ( lline[ 0 ] == '*' )  && ( lline[ 1 ] == '/' ) ) 
           {    area_archive = 0;  export_skip = 1;       }

           if ( fileeof == 0 )
           if ( c == '\n' ) 
           {
             lline[pcc++]='\0';
             printf( "%s", lline );
             printf( "\n" );

             if ( area_archive == 0 )
             {
             if ( export_skip == 0 )
             {
             if (  ( lline[ 0 ] == '/' ) && ( lline[ 1 ] == '/' )  )
             {
                   printf( ">Rem : do nothing.\n" );
             }

             else if ( ( lline[0] == '#' ) && ( strstr(lline, "#include{" ) != 0 ) )
             {
                 //// read
                 strncpy( foostr , strcut( lline , 8+2 , strlen( lline )-1 )  ,  PATH_MAX );

                 //// convert
                 strncpy( filearg ,  foostr , PATH_MAX );
                 if ( strcmp( foostr, "beamer.mrk" ) == 0 ) 
                   strncpy( filearg, "unibmr.mrk", PATH_MAX );

                 //// clone for local check 
                 strncpy( fileargorg , filearg , PATH_MAX );

                 //// process from filearg to ncpfile 
                 strncpy( ncpfile , getenv( "HOME" ), PATH_MAX );
                 strncat( ncpfile , "/include/mrk/" , PATH_MAX - strlen( ncpfile ) -1 );
                 strncat( ncpfile , filearg , PATH_MAX - strlen( ncpfile ) -1 );

                 if ( fexist( fileargorg ) == 1 ) 
                 {
                    strncpy( ncpfile , fileargorg , PATH_MAX );
                    printf( ">>>> NOTE: Take the file include into the pwd path root of elf.\n");
                 }

                 printf( "=> Found an include with string  |%s|%d|\n" , ncpfile , fexist( ncpfile ) );
                 printf( "=> Found an include #include: |%s|%d|\n" ,    ncpfile , fexist( ncpfile ) );

                 if ( fexist( ncpfile ) == 1 ) 
                 {
                   fp5 = fopen( ncpfile  , "r");
                   while( ( ch = fgetc(fp5) ) != EOF )
                   {
                      printf( "%c", ch );
                      fputc( ch, target);
                   }
                   fclose(fp5);
                 }
             }

             }
             }

             // clear
             lline[0]='\0';
             pcc = 0;

           }

           co = c; 
       }
   }
   fclose(source);
   fclose( target );
}




///////////////////////////////////////////
void readfileline( char *fileoutput , char *filesource )
{
   FILE *source; 
   FILE *target; 
   int ch, c, co;
   char lline[PATH_MAX];
   char charo[PATH_MAX];
   int pcc = 0;
   int linecount = 0;
   int posx    = 100;
   int posy    = 730;
   int posxmax = 588;
   int posymax = 835;

   source = fopen( filesource , "r");
   FILE *fp5;
   char ncpfile[PATH_MAX];
   char fileargorg[PATH_MAX];
   char filearg[PATH_MAX];
   char fooline[PATH_MAX];
   char foostr[PATH_MAX];
   int export_skip = 0;
   int area_archive = 0;
   int fileeof = 0;
   int mode_open_frame = 0;
   int mode_frame_level = 0;
   int mode_frame_fig = 0;
   while( fileeof == 0 )
   {
       export_skip = 0;
       c = fgetc(source); 
       if ( c == EOF ) fileeof = 1;
       // EOF

       else
       {

           if ( c != '\n' )
               lline[pcc++]=c;

           if ( ( lline[ 0 ] == '!' )  && ( lline[ 1 ] == 'E' ) && ( lline[ 2 ] == 'O' )  && ( lline[ 3 ] == 'F' )  )
              fileeof = 1;

           if ( ( lline[ 0 ] == '/' )  && ( lline[ 1 ] == '*' ) ) 
               area_archive = 1;

           if ( ( lline[ 0 ] == '*' )  && ( lline[ 1 ] == '/' ) ) 
           {    area_archive = 0;  export_skip = 1;       }

           if ( fileeof == 0 )
           if ( c == '\n' ) 
           {
             lline[pcc++]='\0';
             printf( "%s", lline );
             printf( "\n" );

             if ( area_archive == 0 )
             {
             if ( export_skip == 0 )
             {
             if (  ( lline[ 0 ] == '/' ) && ( lline[ 1 ] == '/' )  )
             {
                   printf( ">Rem : do nothing.\n" );
             }

             else if   ( ( lline[0] == '#' ) && ( lline[1] == ' ' ) ) 
             {
                 // # section
                 strncpy( fooline , strcut( lline, 1+2 , strlen( lline ) )  ,  PATH_MAX );
                 add2frame( "\\section{" );
                 add2frame( fooline );
                 add2frame( "}" );
                 add2frame( "\n" );
                 printf( "=> section\n" );
             }

             else if ( ( lline[0] == '!' ) && ( strstr(lline, "!section " ) != 0 ) )
             {
                 // !section section
                 strncpy( fooline , strcut( lline, 1+2+7 , strlen( lline ) )  ,  PATH_MAX );
                 add2frame( "\\section{" );
                 add2frame( fooline );
                 add2frame( "}" );
                 add2frame( "\n" );
                 printf( "=> section\n" );
             }

             else if ( ( lline[0] == '#' ) && ( lline[1] == '#' ) && ( lline[2] == ' ' ) )
             {
                 strncpy( fooline , strcut( lline, 2+2 , strlen( lline ) )  ,  PATH_MAX );
                 add2frame( "\\subsection{" );
                 add2frame( fooline );
                 add2frame( "}" );
                 add2frame( "\n" );
                 printf( "=> subsection\n");
             }

             else if ( ( lline[0] == '!' ) && ( strstr(lline, "!clr" ) != 0 ) )
             {
                if ( lineid >= 2 ) 
                {
                   printf( "=> Export frame to file, type 1.\n" );
                   printframe( );
                }

                printf( "=> clearpage or new frame.\n" );
                printf( "=> puts \\begin{frame}\n" );
                 mode_open_frame = 1;
                 mode_frame_level = 0;
                 add2frame( "\\begin{frame}\n" );
                 add2frame( "\\fitimage{\n" );
                 add2frame( "\\frametitle{\\thesection.~\\insertsection}\n"   );
                 add2frame( "\\begin{itemize}\n" );
             }

             else if ( ( lline[0] == '>' ) && ( lline[1] == ' ' ))
             {
                 printf( "=> > item into frame.\n" );
                 if ( mode_frame_level == 2 ) 
                 {
                    add2frame( "\\end{itemize}\n" );
                 }
                 mode_frame_level = 1;
                 add2frame( "\\item "  );
                 strncpy( foostr , strcut( lline , 1+2 , strlen( lline ) )  ,  PATH_MAX );
                 printf( ">Place |%s| into file.\n" , foostr );
                 add2frame( foostr   );
                 add2frame( "\n"   );
             }

             else if ( ( lline[0] == '-' ) && ( lline[1] == ' ' ))
             {
                 printf( "=> - item into frame.\n" );
                 if ( mode_frame_level == 1 ) 
                 {
                    add2frame( "\\begin{itemize}\n"  );
                 }
                 mode_frame_level = 2; 
                 add2frame( "\\item "  );
                 strncpy( foostr , strcut( lline , 1+2 , strlen( lline ) )  ,  PATH_MAX );
                 add2frame( foostr  );
                 printf( ">Place |%s| into file.\n" , foostr );
                 add2frame( "\n"   );
             }


             else if ( ( lline[0] == '!' ) && ( strstr(lline, "!gpath{~/" ) != 0 ) )
             {
                 printf( "=> graphicspath.\n" );
                 strncpy( foostr , strcut( lline , 1+3+2+2+2 , strlen( lline )-1 )  ,  PATH_MAX );
                 add2frame( "\\graphicspath{{"  );
                 add2frame( getenv("HOME")  );
                 add2frame( "/"  );
                 add2frame( foostr  );
                 add2frame( "}}"  );
                 add2frame( "\n" );
             }

             else if ( ( lline[0] == '!' ) && ( strstr(lline, "!fig{" ) != 0 ) )
             {
                 strncpy( foostr , strcut( lline , 1+3+2 , strlen( lline )-1 )  ,  PATH_MAX );
                 printf( "=> fig figure |%s| into frame.\n" , foostr );
                 printf( "=> Place end itemize\n" );
                 printf( "=> Place end itemize\n" );
                 add2frame( "\\end{itemize}\n"  );
                 add2frame( "\\end{itemize}\n" );
                 add2frame( "}{"  );
                 add2frame( foostr );
                 add2frame( "}[]\n" );
                 add2frame( "\\end{frame}\n" );
                 add2frame( "\n"  );
                 add2frame( "\n"  );
                 mode_open_frame = 0;
                 printf( "=> Close frame, type 1.\n" );
                 mode_open_frame = 0;
                 mode_frame_level = 0;
                 mode_frame_fig = 0;

                 printf( "=> Export frame to file, type 1.\n" );
                 printframe( );

                 strncpy( filearg , fbasename( strcut( lline , 1+3+2 , strlen( lline )-1 ) )  ,  PATH_MAX );
                 strncpy( ncpfile , getenv( "HOME" ), PATH_MAX );
                 strncat( ncpfile , "/pool/figs/" , PATH_MAX - strlen( ncpfile ) -1 );
                 strncat( ncpfile , filearg , PATH_MAX - strlen( ncpfile ) -1 );
                 printf( "=> Check of Fig into pool figs, with string  |%s|%d|\n" , ncpfile , fexist( ncpfile ) );
             }


             else if ( ( strcmp( lline, "" ) == 0 ) && ( mode_frame_level >= 1 ) )
             {
                 printf( "=> Note here a frame to close\n" );
                 if ( mode_frame_level == 2 ) 
                 {
                    add2frame( "\\end{itemize}\n" );
                    add2frame( "\\end{itemize}\n" );
                 }
                 else if ( mode_frame_level == 1 ) 
                 {
                    add2frame( "\\end{itemize}\n"  );
                 }
                 add2frame( "\\end{frame}\n"   );
                 add2frame( "\n"  );
                 add2frame( "\n"  );
                 printf( "=> Close frame, type 2.\n" );
                 mode_open_frame = 0;
                 mode_frame_level = 0;
                 mode_frame_fig = 0;

                 printf( "=> Export frame to file, type 2.\n" );
                 printframe( );
             }


             else if ( ( lline[0] == '!' ) && ( strstr(lline, "!begin" ) != 0 ) )
             {
                 printf( "=> begin document\n" );
                 add2frame( "\n" );
                 add2frame( "\\begin{document}" );
                 add2frame( "\n" );
                 add2frame( "\n" );
             }

             else if ( ( lline[0] == '!' ) && ( strstr(lline, "!enddoc" ) != 0 ) )
             {
                 printf( "=> enddoc\n" );
                 add2frame( "\n" );
                 add2frame( "\\end{document}" );
                 add2frame( "\n" );
                 add2frame( "\n" );
             }

             else if ( ( lline[0] == '!' ) && ( strstr(lline, "!bigskip" ) != 0 ) )
             {
                 add2frame( "\\bigskip" );
                 add2frame( "\n" );
                 printf( "=> bigskip\n" );
             }


             else if ( lline[0] == '!' ) 
             {
                 printf( "=> not found, cmd, just do nothing.\n" );
             }

             else if ( ( lline[0] == '/' ) && ( lline[1] == '/' ) ) 
             {
                   printf( ">Rem : do nothing.\n" );
             }
             else
             {
               //fputs( lline , target);
               //fputs( "\n" , target);
             }
             }
             }

             // clear
             lline[0]='\0';
             pcc = 0;

           }

           co = c; 
       }
   }
   fclose(source);
}









int main( int argc, char *argv[])
{
    char fichier[PATH_MAX];
    char targetfile[PATH_MAX];
    char string[PATH_MAX];

    if ( argc == 2)
    if ( strcmp( argv[1] , "time" ) ==  0 ) 
    {
       printf("%d\n", (int)time(NULL));
       return 0;
    }

    ////////////////////////////////////////////////////////
    if ( argc == 3)
    if ( strcmp( argv[1] , "-f" ) ==  0 ) 
    {
       printf("%syellow\n", KYEL);
       readfile( argv[ 2 ] );
       return 0;
    }
   

    struct winsize w; // need ioctl and unistd 
    ioctl( STDOUT_FILENO, TIOCGWINSZ, &w );
    printf("Env HOME:  %s\n", getenv( "HOME" ));
    printf("Env PATH:  %s\n", getcwd( string, PATH_MAX ) );
    printf("Env TERM ROW:  %d\n", w.ws_row );
    printf("Env TERM COL:  %d\n", w.ws_col );

    ///////////////
    if ( argc == 1)
    {
       printf("Usage: please enter a file to use this software." );
       printf("\n" );
       return 0;
    }

    ///////////////
    if ( argc == 2)
      strncpy( fichier, argv[ 1 ] , PATH_MAX );

    ////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
    if ( argc == 2)
    {
          strncpy( targetfile, fbasenoext( argv[ 1 ] ) , PATH_MAX );
          strncat( targetfile , ".tex" , PATH_MAX - strlen( targetfile ) -1 );
          printf( "> Target: %s\n" , targetfile );
          printf( "> Source: %s => Target: %s \n", argv[1] , targetfile  );
          printf( "\n" );
          printf( "\n" );
          strncpy( targettexfile, targetfile, PATH_MAX );
          includefile( targetfile, argv[ 1 ] );
          readfileline( targetfile, argv[ 1 ] );
          printend( );
          printf( "\n" );
          printf( "> Target: %s\n" , targetfile );
          printf( "> Source: %s => Target: %s \n", argv[1] , targetfile  );
          printf( "> Target: %s\n" , targetfile );
          printf( "> COMPLETED.\n" );
          printf( "\n" );
    }
    return 0;
}






