	//alert("你好！");
	

//var d=Math.round(10*(Math.random()));
//alert(d);
	
//执行变色
function rnum(){
	
	
//随机获取三个格子的id
var a,b,c;
do{ a=Math.round(10*(Math.random()));
	b=Math.round(10*(Math.random()));
	c=Math.round(10*(Math.random()));
   }
	while((a==b)||(b==c)||(a==c)||(a>9)||(b>9)||(c>9));
var j=("box"+a).toString();
var k=("box"+b).toString();
var l=("box"+c).toString();
 	
//alert(box10);



//alert(getRandomColor());
var a1,b1,c1;
do{
	a1=getRandomColor();
	b1=getRandomColor();
	c1=getRandomColor();
}
	while((a1==b1)||(b1==c1)||(a1==c1));
	var a1=a1.toString();
	var b1=b1.toString();
	var c1=c1.toString();	
	
	toyellow();
/*	for(var i=1;i<10;i++){
	var u=("box"+i).toString();
	document.getElementById(u).style.backgroundColor="#F3B613" ;
}*/
	//for(;;)
	document.getElementById(j).style.backgroundColor=a1 ;
	document.getElementById(k).style.backgroundColor=b1 ;
	document.getElementById(l).style.backgroundColor=c1 ;
	
}


//点击启动闪
function onclick2(){
		  
		 
		 	id1=window.setInterval("rnum()",1000);			 
}

//点击停止闪
function stoprnum(){
	window.clearInterval(id1);
	//alert("box10");
	toyellow();
}

//延时执行
function sTime(){
		document.getElementById("button2").click();
		onclick2();
	}

//历遍黄色
function toyellow(){
		for(var i=1;i<10;i++){
	var u=("box"+i).toString();
	document.getElementById(u).style.backgroundColor="#F3B613" ;
}
}


//随机获取三种颜色值
function getRandomColor(){
return "#"+("00000"+((Math.random()*16777215+0.5)>>0).toString(16)).slice(-6);
} 

//鼠标移入动作
function toflicker(){
	document.getElementById("button1").style.backgroundColor="#FFC125";
}
function toflicker1(){
	document.getElementById("button2").style.backgroundColor="#FFC125";
}

//鼠标移出动作
function tohint(){
document.getElementById("button1").style.backgroundColor="#FFFFFF";	
document.getElementById("button2").style.backgroundColor="#FFFFFF";
}





