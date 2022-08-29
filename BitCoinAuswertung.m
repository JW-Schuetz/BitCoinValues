function BitCoinAuswertung()
    clc
    clear

    d = load( 'data.mat' );

    m = 1;
    for n = 1 : length( d.p )
        if( d.t( n ) > datetime( 2019, 7, 10 ) )
            if( ~isnan( d.p( n ) ) )
                price( m ) = d.p( n );
                m = m + 1;
            end
        end
    end

    corr = xcorr( price );

    hold on
    %plot( price )
    plot( corr )
end