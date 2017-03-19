A = imrotate(imread('../Data/anton.jpg'), -90);
image(A);

hold on


y_line_0 = [1821 3169; 1342 1998];
y_line_1 = [1527 1997; 2269 3175];
y_line_2 = [59 3239;   641  2023];
x_line_0 = [94 2012;   1505 1994];
x_line_1 = [89 3185;   2262 3166];
z_line_0 = [1458 1904;  1477 617];
z_line_1 = [645 1900;    631 965];
z_line_2 = [290 1904;    265 952];
h_line   = [1269 1972; 1273 1192];

line(x_line_0(:,1), x_line_0(:,2), 'Color','blue','LineStyle',':','LineWidth',2);
line(x_line_1(:,1), x_line_1(:,2), 'Color','blue','LineStyle',':','LineWidth',2);
line(y_line_0(:,1), y_line_0(:,2), 'Color','blue','LineStyle',':','LineWidth',2);
line(y_line_1(:,1), y_line_1(:,2), 'Color','blue','LineStyle',':','LineWidth',2);
line(y_line_2(:,1), y_line_2(:,2), 'Color','blue','LineStyle',':','LineWidth',2);
line(z_line_0(:,1), z_line_0(:,2), 'Color','blue','LineStyle',':','LineWidth',2);
line(z_line_1(:,1), z_line_1(:,2), 'Color','red','LineStyle', '-','LineWidth',2);
line(z_line_2(:,1), z_line_2(:,2), 'Color','blue','LineStyle',':','LineWidth',2);
line(h_line(:,1),   h_line(:,2),   'Color','red','LineStyle', '-','LineWidth',2);

% x axis
[x,y] = intersect_(x_line_0, x_line_1);
text(0, 40,  ['X vanishing point: (', num2str(floor(x)),', ',num2str(floor(y)), ')'],'Color','green','FontSize',10,'FontWeight','bold');
text(x, y, ['\leftarrow X vanishing point: (', num2str(floor(x)),', ',num2str(floor(y)), ')'],'Color','green','FontSize',10,'FontWeight','bold');
display(['Vanishing point on x axis: (', num2str(x),', ',num2str(y), ')']);


% z axis
[x,y] = intersect_(z_line_0, z_line_1);
text(0, 120,  ['Z vanishing point: (', num2str(floor(x)),', ',num2str(floor(y*10)), ')'],'Color','green','FontSize',10,'FontWeight','bold');
text(x, y, ['\leftarrow Z vanishing point: (', num2str(floor(x)),', ',num2str(floor(y)), ')'],'Color','green','FontSize',10,'FontWeight','bold');
display(['Vanishing point on z axis: (', num2str(x),', ',num2str(y*10), ')']);

% y axis
[x,y] = intersect_(y_line_0, y_line_2);
text(0, 80,  ['Y vanishing point: (', num2str(floor(x)),', ',num2str(floor(y)), ')'],'Color','green','FontSize',10,'FontWeight','bold');
display(['Vanishing point on y axis: (', num2str(x),', ',num2str(y), ')']);
line([0 10000], [y y], 'Color','yellow','LineStyle',':','LineWidth',2);
line([x,y_line_0(2,1)],[y,y_line_0(2,2)], 'Color','white','LineStyle','--','LineWidth',0.75);
line([x,y_line_1(1,1)],[y,y_line_1(1,2)], 'Color','white','LineStyle','--','LineWidth',0.75);
line([x,y_line_2(2,1)],[y,y_line_2(2,2)], 'Color','white','LineStyle','--','LineWidth',0.75);
horizon = [0 y; 10000 y];
v_ = [x;y;1];


% intersecting b1-b2 with horizon
b1b2 = [h_line(1,1) h_line(1,2); z_line_1(1,1) z_line_1(1,2)];
[ux,uy] = intersect_(b1b2, horizon);
text(0, uy + 60, ['U: (', num2str(floor(ux)),', ',num2str(floor(uy)), ')'],'Color','green','FontSize',10,'FontWeight','bold');
line([h_line(1,1) ux],[h_line(1,2) uy], 'Color','white','LineStyle','--','LineWidth',0.75);
display(['u: (', num2str(ux),', ',num2str(uy), ')']);

% intersecting u-t1 with door
ut1 = [ux uy; h_line(2,1) h_line(2,2)];
[t2x,t2y] = intersect_(ut1, z_line_1);
text(t2x, t2y - 30, ['t2: (', num2str(floor(t2x)),', ',num2str(floor(t2y)), ')'],'Color','green','FontSize',10,'FontWeight','bold');
line([ux h_line(2,1)],[uy h_line(2,2)], 'Color','white','LineStyle','--','LineWidth',0.75);
display(['t2: (', num2str(t2x),', ',num2str(t2y), ')']);

text(h_line(1,1), h_line(1,2), 'b1','Color','green','FontSize',10,'FontWeight','bold');
text(h_line(2,1), h_line(2,2), 't1','Color','green','FontSize',10,'FontWeight','bold');
text(z_line_1(1,1), z_line_1(1,2), 'b2','Color','green','FontSize',10,'FontWeight','bold');

d2 = l_(z_line_1(1,1), z_line_1(1,2), z_line_1(2,1), z_line_1(2,2));
d1 = l_(z_line_1(1,1), z_line_1(1,2), t2x, t2y);
display('d1 and d2:');
display(d1);
display(d2);

h1 = 1780;
h2 = h1*d2/d1;
display('h2:');
display(h2);


%--------------------------------------------------------------
f = 4;
K = [f 0 size(A,2)/2;
0 f size(A,1)/2;
0 0 1];


r3 = inv(K)*v_/norm(inv(K)*v_)
pitch = atan(-r3(1)/norm(r3(2:3)))
roll = atan(r3(2)/r3(3))


function z = l_(x1, y1, x2, y2)
z = sqrt((x1-x2)^2 + (y1-y2)^2);
end

function [x, y] = intersect_(l1, l2)
x1 = l1(1,1);
y1 = l1(1,2);
x2 = l1(2,1);
y2 = l1(2,2);
x3 = l2(1,1);
y3 = l2(1,2);
x4 = l2(2,1);
y4 = l2(2,2);
x = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
y = ((x1*y2-y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
end