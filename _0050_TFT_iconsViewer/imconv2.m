% matlab octave image to c-file converter
pkg load image
fileID = fopen('images.c','w');

%image to byte array converter
a=imread("fly.png");
b=imresize(a,0.4);

# test cross image
if 0
  clear b
  b=zeros(15,9,3);
  b(1,:,1)=ones(9,1);
  b(15,:,1)=ones(9,1);
  b(:,1,1)=ones(15,1);
  b(:,9,1)=ones(15,1);
  b(8,:,3)=ones(9,1);
  b(:,5,3)=ones(15,1);
  b=b*255;
end

if 0
  clear b
  # h=2, w=3
  b=zeros(2,3,3);
  b(1,:,1)=ones(3,1)*255; % horizontal line
  b(:,1,1)=ones(2,1)*128; % vertikal line
  
end



# octave matrix a(y,x) 
# x and y has changed order compared to usual cartessian
# coordinates
h=length(b(:,1,1))
w=length(b(1,:,1))
d=w*h;

fprintf(fileID,"#define IMAGE_WIDTH %d\n\r",w);
fprintf(fileID,"#include \"images.h\"\n\r");
fprintf(fileID,"ImageDirectoryEntry_t ImageDirectory[]={\n\r");
fprintf(fileID,"{");
fprintf(fileID,"RGB,");
address=0;
fprintf(fileID,"%d,",address);
fprintf(fileID,"%d,",w);
fprintf(fileID,"%d}\n\r",h);
fprintf(fileID,"};\n\r");

lineLength=32;
l=lineLength;
fprintf(fileID,"const uint8_t images[]={\n\r");
for farbe=1:3,
  for hh=1:h,    
    for ww=1:w,
     fprintf(fileID,"%d",b(hh,ww,farbe));
     if ww*hh*farbe~=w*h*3,
        fprintf(fileID,",");
     end
    end
    fprintf(fileID,"\n\r");
  end

end

fprintf(fileID,"\n\r};\n\r");
fclose(fileID);
