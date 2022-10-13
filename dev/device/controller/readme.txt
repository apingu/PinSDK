Input time window  ( unit 1/1000s )

start point

command list


/*
if( msg == start point )
{
    setcycletime( )
}
*/

if( now command list == [0] )
{
    setcycletime( )
}

if( now < cycletime )
{
    a = check anim list();
    do anim( a );
}
else
{
    push(msg);
}
