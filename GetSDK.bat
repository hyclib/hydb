xcopy "..\..\..\hy_bin\latest\include\hyc\*.h"  	"ext\include\hyc\" /S /Y
xcopy "..\..\..\hy_bin\latest\include\hyc\*.hpp"  	"ext\include\hyc\" /S /Y
xcopy "..\..\..\hy_bin\latest\libs\win32\hyc.*"  	"bin\"   /S /Y

xcopy "ext\mysql\lib\opt\libmysql.dll"  		"bin\Debug\"   /S /Y
xcopy "ext\mysql\lib\opt\libmysql.dll"  		"bin\Release\"   /S /Y
xcopy "ext\mysql\lib\opt\libmysql.dll"  		"bin\Debug.unicode\"   /S /Y
xcopy "ext\mysql\lib\opt\libmysql.dll"  		"bin\Release.unicode\"   /S /Y

