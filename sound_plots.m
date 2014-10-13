t=offset:1/8:offset+40 -1/8;

figure(1)
if ((offset*0.001+0.04)*8000<size(x,2))
 	plot(t,raw(offset*8:(40+offset)*8-1));
else 
	plot(t,raw(size(x,2)-320:size(x,2)));
end
title("Senal raw");
xlabel("Tiempo [ms]");
ylabel("Amplitud");

figure(2);
if ((offset*0.001+0.04)*8000<size(x,2))
 	plot(t,sat(offset*8:(40+offset)*8-1));
else 
	plot(t,sat(size(x,2)-320:size(x,2)));
end
title("Senal Distorsionada");
xlabel("Tiempo [ms]");
ylabel("Amplitud");


figure(3);
if ((offset*0.001+0.04)*8000<size(x,2))
 	plot(t,x(offset*8:(40+offset)*8-1));
else 
	plot(t,x(size(x,2)-320:size(x,2)));
end
title("Senal Aproximada");
xlabel("Tiempo [ms]");
ylabel("Amsplitud");