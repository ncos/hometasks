function [y1, y2, in] = visualize_matches(image_count, M, xm, xy)
k = 1;
thr = 8;

%Image 1
for i = 1 : image_count - 1 
    % Every other Image that have common features with Image 1
    for j = i + 1 : image_count 
        subplot(4,3,k); hold on; 
   
        index1 = find(M(:,i) == 1);
        index2 = find(M(:,j) == 1);
        
        % Find the intersect between the two indexes:
        index = intersect(index1, index2);
        filename = sprintf('%d.jpg', i);
        I = imread(filename);
        len = floor(length(I(1,:))/3);
        image(0, 0, I);
        
        % Second File:
        filename = sprintf('%d.jpg', j);
        I = imread(filename);
        image(len, 0, I);
  
        % Limit the x and y axis:
        xlim([0 len*2])
        ylim([0 floor(length(I(:,1)))])
  
        % Reverse the Y Direction, image was upside-down:
        set(gca,'YDir','reverse');
        x1 = [xm(index,i) xy(index,i)];
        x2 = [xm(index,j) xy(index,j)];
        
        % Set up the threshold on size:
        if size(x1,1) < thr
            continue;
        end
        
        % Plot matches (Without Outlier Rejection):
        for t = 1:length(x1)
            plot([x1(t,1) x2(t,1)+len], [x1(t,2) x2(t,2)], 'r:');
        end
        
        % Call GetInliersRANSAC for Outlier Rejection
        [y1, y2, in] = GetInliersRANSAC(x1, x2);
        M(index(~in),i) = 0;
   
        % Visualization:
        text(0, -40, sprintf('Image %d', j));
        text(2250, -40, sprintf('Image %d', i));
        for t = 1:length(y1)
            plot([y1(t,1) y2(t,1)+len], [y1(t,2) y2(t,2)], 'g-');
        end
        hold off;
        k = k + 1;    
    end
end
