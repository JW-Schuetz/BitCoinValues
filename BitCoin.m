function BitCoin()
    clc
    clear

    % Es gibt zwei Modi:
    %   1. alle Dateien auswerten und abspeichen (das kann etwas dauern)
    %	2. nur die Neuen auswerten und nicht abspeichern (geht schneller)
    Modus = 2;

    switch( Modus )
        case 1
        case 2

        otherwise
            error( 'Modus: illegaler Wert!' )
    end

    KaufKurs   = 0;     % Kurs zu dem ich Bitcoins gekauft habe
    sampleTime = 15;	% 15 Minuten Samplingzeit

    % Directory-Einträge
    shortNames = {};
    fileNames  = {};

    d = dir();
    D = length( d );
    for n = 1 : D
        % Verzeichnis?
        if( ~d( n ).isdir() )   % falls kein Verzeichnis: weiter
            continue
        end

        % Verzeichnis mit richtiger Namensstruktur?
        ndx = regexp( d( n ).name, '^20[0-9][0-9]$', 'once' );
        if( isempty( ndx ) )    % Verzeichnis mit falscher Namensstruktur: weiter
            continue
        end

        % Verzeichnis mit richtiger Namensstruktur
        sd = dir( [ d( n ).name, '/*.csv' ] );
        SD = length( sd );

        for m = 1 : SD
            name = sd( m ).name;

            % Dateiname mit richtiger Namensstruktur?
            ndx = regexp( name, '^BitCoinPrice-[0-9]{2}.[0-9]{2}.20[0-9]{2}.csv$', 'once' );
            if( isempty( ndx ) )    % Verzeichnis mit falscher Namensstruktur: weiter
                continue
            end

            name = strsplit( name, '-' );
            name = strsplit( name{ 2 }, '.csv' );
            name = strsplit( name{ 1 }, '.' );
    
            shortNames = [ shortNames, strcat( name{ 3 }, name{ 2 }, name{ 1 } ) ]; %#ok<AGROW> 
            fileNames  = [ fileNames, [ d( n ).name, '/', sd( m ).name ] ]; %#ok<AGROW> 
        end
    end

    % sortieren, interessant ist nur der Dateiname
    [ shortNames, ndx ] = sort( shortNames );
    fileNames = fileNames( ndx );

    switch( Modus )
        case 1
            % nichts zu tun

        case 2
            % schon verarbeitete Daten einlesen
            load( 'BitCoinPrice.mat', 'tSaved', 'pSaved' )

            % Zeitstempel des letzten Eintrages in tSaved
            ts = tSaved( end ); 

            % Datum der letzten verarbeiteten Datei
            shortNameLastSaved = sprintf( '%d%d%d', ts.Year, ts.Month, ts.Day );

            % nur die neueren Dateien weiter beachten
            fnNew   = "";
            counter = 0;
            for n = 1 : length( shortNames )
                if( string( shortNames{ n } ) >= string( shortNameLastSaved ) )
                    counter = counter + 1;
                    fnNew( counter ) = fileNames{ n };
                end
            end

            % Ergebnis aufbereiten
            fileNames = fnNew;
    end

	t = datetime;   % Initialisierung
	p = [];

    counter = 1;
    for n = 1 : length( fileNames )
        fd = fopen( fileNames{ n }, 'r' );

        fileData = textscan( fd, '%s %s %f' );

        for m = 1 : length( fileData{ 1 } )
            time = strsplit( fileData{ 2 }{ m }, ',' );
            dt   = datetime( [ fileData{ 1 }{ m }, ' ', time{ 1 } ], ...
                   'InputFormat', 'dd.MM.yyyy HH:mm' );

            % keine Doppelteinträge erlauben
            if( counter == 1 || ( counter ~= 1 && dt ~= t( counter - 1 ) ) )
                t(counter) = dt;
                p(counter) = fileData{ 3 }( m ); %#ok<AGROW>
                counter = counter + 1;
            end
        end

        fclose( fd );
    end

	% ... u.a. fehlende Datensätze mit NaN aufüllen (ohne Rand aus tSaved)
	[ t, p ] = refineData( sampleTime, t, p );

    switch( Modus )
        case 1
            if( ~isempty( t ) && ~isempty( p ) )
                tSaved = t;
                pSaved = p;
                save( 'BitCoinPrice.mat', 'tSaved', 'pSaved' )
            end

        case 2
            %
            %      tSaved                                       tMod
            %  -------------                                -------------
            % |      1      |                              |      1      |
            %  -------------                                -------------
            % |      2      |              t               |      2      |
            %  -------------         -------------          -------------
            % |     ...     |       |      1      |        |      3      |
            %  -------------         -------------          -------------
            % |     end     | <-    |      2      |        |      4      |
            %  -------------    |    -------------          -------------
            %                    -> |     ndx     |        |      5      |
            %                        -------------          -------------
            %                       |     ...     |        |     ...     |
            %                        -------------          -------------
            %                       |     end     |        |     end     |
            %                        -------------          -------------
            %
            if( ~isempty( t ) && ~isempty( p ) )
                ndx = find( t == ts );

                % alle t hinter t(ndx) sind neu
                t   = [ tSaved, t( ndx + 1 : length( t ) ) ];
                p   = [ pSaved, p( ndx + 1 : length( p ) ) ]; 
            else
                t = tSaved;
                p = pSaved; 
            end
    end

    % Plotten
    f = figure( 1 );
    f.Name = 'BitCoin Kurs';

    xlabel( 'Zeit', 'FontSize', 12, 'FontWeight', 'normal' )
    ylabel( 'Kurs [Euro]', 'FontSize', 12, 'FontWeight', 'normal' )
    ylim( [ 3000, 65000 ] )

    grid on
    hold on

    plot( t, p, 'b', 'linewidth', 0.5 )
    if( KaufKurs ~= 0 )
        plot( [ t( 1 ); t( end ) ], [ KaufKurs; KaufKurs ], 'r', 'linewidth', 1.0 )
    end
	legend( 'BitCoinKurs', 'Location', 'northwest' )
end

function [ t, p ] = refineData( sampleTime, tRaw, pRaw )
    if( isempty( tRaw ) || isempty( pRaw ) )
        t = [];
        p = [];
        return
    end

    % Daten äquidistant machen und evtl. mit NaN auffüllen, Doppelteinträge ignorieren
    t = tRaw;
    p = pRaw;

    counter = 1;
    repeate = true;
    while( repeate )
        [ repeate, t, p ] = substitute( sampleTime, t, p, counter );
        counter = counter + 1;
    end

    % zeitlich sortieren
    [ t, ndx ] = sort( t );
    p          = p( ndx );
end

function [ changed, t, p ] = substitute( sampleTime, t, p, ~ )
    [ missingTimes, missingPrices, n ] = detect( sampleTime, t );

    % Es gibt 2 Fälle:
    % missingTimes und missingPrices leer: alles OK und das liefert changed = false
    % missingTimes und missingPrices nichtleer: Lücke entdeckt und das liefert changed = true
    changed = false;

    if( ~isempty( missingPrices ) && ~isempty( missingTimes ) )
        preT  = t( 1 : n );
        postT = t( n + 1 : end );
        t = [ preT, missingTimes, postT ];

        preP  = p( 1 : n );
        postP = p( n + 1 : end );
        p = [ preP, missingPrices, postP ];

        changed = true;
    end
end

function [ missingTimes, missingPrices, n ] = detect( sampleTime, t )
    % Daten äquidistant machen und evtl. mit NaN auffüllen
    % Einträge mit einer Zeitdifferenz von 0 oder kleiner werden ignoriert

    % Es gibt 2 Fälle für die zurückgegebenen Daten:
    % missingTimes und missingPrices leer und n = 0: alles OK
    % missingTimes und missingPrices nichtleer: Lücke entdeckt

	missingTimes  = [];
	missingPrices = [];

    for n = 1 : length( t ) - 1
        delta = minutes( t( n + 1 ) - t( n ) );
        if( delta ~= sampleTime )
            if( delta > sampleTime )
                % Lücke entdeckt, fehlende Daten generieren
                missingSamples = delta / sampleTime;
                missingTimes   = datetime;   % Initialisierung

                for m = 1 : missingSamples
                    missingTimes( m )  = datetime( t( n ) + m * minutes( sampleTime ) );
                    missingPrices( m ) = NaN; %#ok<AGROW>
                end

                return
            end
        end
    end
end