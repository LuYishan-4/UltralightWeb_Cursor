let mouseDown=false;



function clickEffect()
{

    const ring =
        document.createElement(
            "div"
        );


    ring.className="ring";


    document.body.appendChild(
        ring
    );


    setTimeout(
        ()=>ring.remove(),
        600
    );

}





function particle()
{


    const p =
        document.createElement(
            "div"
        );


    p.className="particle";



    let x =
        (Math.random()-0.5)*80;


    let y =
        (Math.random()-0.5)*80;



    p.style.left="64px";

    p.style.top="64px";



    p.style.setProperty(
        "--x",
        x+"px"
    );


    p.style.setProperty(
        "--y",
        y+"px"
    );



    document.body.appendChild(
        p
    );


    setTimeout(
        ()=>p.remove(),
        600
    );

}





window.moveCursor =
function(x,y,pressed)
{


    if(
        pressed &&
        !mouseDown
    )
    {

        clickEffect();


        for(
            let i=0;
            i<8;
            i++
        )
        {
            particle();
        }

    }



    mouseDown=pressed;



    if(pressed)
    {
        particle();
    }


}