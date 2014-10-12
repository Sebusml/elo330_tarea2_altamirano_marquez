offset=2;
distort=rand(1,32281);
int16(distort);
original=rand(1,32281);
int16(original);


t=offset:1/8:offset+40 -1/8;

x=distort;
max_x=max(x);
min_x=min(x);
index_max=find(x==max_x);
index_min=find(x==min_x);


first_index=index_max(1);

for i = 2:size(index_max,2)
	if (index_max(i)==index_max(i-1)+1)
		last_index=index_max(i);
	else
		y=polyfit([first_index-2 first_index-1 last_index+1 last_index+2],[x(first_index-2) distort(first_index-1) distort(first_index+1) distort(first_index+2)],2);
 		z=polyval(y,first_index:last_index);
 		x(first_index:last_index)=z;
		first_index=index_max(i);
	end
 	
 end
 
 y=polyfit([first_index-2 first_index-1 last_index+1 last_index+2],[x(first_index-2) distort(first_index-1) distort(first_index+1) distort(first_index+2)],2);
 z=polyval(y,first_index:last_index);
 x(first_index:last_index)=z;

first_index=index_min(1);

 for i = 2:size(index_min,2)
	if (index_min(i)==index_min(i-1)+1)
		last_index=index_min(i);
	else
		y=polyfit([first_index-2 first_index-1 last_index+1 last_index+2],[x(first_index-2) distort(first_index-1) distort(first_index+1) distort(first_index+2)],2);
 		z=polyval(y,first_index:last_index);
 		x(first_index:last_index)=z;
		first_index=index_min(i);
	end
 	
 end
 
 y=polyfit([first_index-2 first_index-1 last_index+1 last_index+2],[x(first_index-2) distort(first_index-1) distort(first_index+1) distort(first_index+2)],2);
 z=polyval(y,first_index:last_index);
 x(first_index:last_index)=z;

int16(x);
size(original(offset*8:(40+offset)*8),2)
figure(1)
if ((offset*0.001+0.04)*8000<size(x,2))
 	plot(t,original(offset*8:(40+offset)*8-1));
else 
	plot(t,original(size(x,2)-320:size(x,2)));
end
title("Senal Original");
xlabel("Tiempo [ms]");
ylabel("Amplitud");

figure(2);
if ((offset*0.001+0.04)*8000<size(x,2))
 	plot(t,distort(offset*8:(40+offset)*8-1));
else 
	plot(t,distort(size(x,2)-320:size(x,2)));
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