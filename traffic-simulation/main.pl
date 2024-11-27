use strict;
use warnings;
use Tk;
use Time::HiRes qw(time);  # Importar la función time de alta resolución

# Definir constantes para los estados del semáforo
use constant {
    GREEN  => 'Green',
    YELLOW => 'Yellow',
    RED    => 'Red',
};

# Definir la clase del semáforo
package TrafficLight;

use constant {
    GREEN  => 'Green',
    YELLOW => 'Yellow',
    RED    => 'Red',
};

sub new {
    my ($class, $green_time, $yellow_time, $red_time, $initial_state, $offset) = @_;
    return bless {
        state       => $initial_state,  # Establecer el estado inicial (Verde para A, Rojo para B)
        green_time  => $green_time,
        yellow_time => $yellow_time,
        red_time    => $red_time,
        offset      => $offset,  # Desfase para sincronizar los semáforos
        timer       => 0,  # Rastrea el tiempo pasado en el estado actual
    }, $class;
}

sub update {
    my ($self, $current_time) = @_;
    
    # Determinar la fase del semáforo basado en el reloj compartido y el desfase
    my $cycle_time = $self->{green_time} + $self->{yellow_time} + $self->{red_time};
    my $time_in_cycle = ($current_time + $self->{offset}) % $cycle_time;
    
    if ($time_in_cycle < $self->{green_time}) {
        $self->{state} = GREEN;  # Asignar el valor de cadena para Verde
    } elsif ($time_in_cycle < $self->{green_time} + $self->{yellow_time}) {
        $self->{state} = YELLOW;  # Asignar el valor de cadena para Amarillo
    } else {
        $self->{state} = RED;  # Asignar el valor de cadena para Rojo
    }
}

sub get_state {
    my ($self) = @_;
    return $self->{state};
}

# Definir la clase del coche
package Car;

sub new {
    my ($class, $lane) = @_;
    my $speed = int(rand(5) + 5);  # Velocidad aleatoria entre 5 y 10
    return bless {
        lane   => $lane,
        x      => $lane == 0 ? 0 : 200,  # Posición inicial para Este o Norte
        y      => $lane == 1 ? 0 : 200,
        speed  => $speed,
    }, $class;
}

sub update {
    my ($self, $traffic_light_state, $light_position) = @_;

    # Determinar si el coche ha alcanzado o pasado el semáforo
    my $has_reached_light = ($self->{lane} == 0 && $self->{x} + $self->{speed} >= $light_position) ||
                            ($self->{lane} == 1 && $self->{y} + $self->{speed} >= $light_position);

    # Imprimir el estado actual del coche y el semáforo
    print "Coche en carril $self->{lane}, posición ($self->{x}, $self->{y}), velocidad $self->{speed}\n";
    print "Estado del semáforo: $traffic_light_state, posición del semáforo: $light_position\n";
    print "¿Ha alcanzado el semáforo? " . ($has_reached_light ? "Sí" : "No") . "\n";

    # Ajustar la velocidad según el estado del semáforo
    my $current_speed = $self->{speed};
    if ($traffic_light_state eq TrafficLight::YELLOW) {
        $current_speed /= 2;  # Reducir la velocidad a la mitad cuando el semáforo está en amarillo
        print "Semáforo amarillo, velocidad reducida a $current_speed\n";
    }

    # Solo moverse si el semáforo está en verde, o si el coche no ha alcanzado el semáforo, o si ya lo ha pasado
    if ($traffic_light_state eq TrafficLight::GREEN || !$has_reached_light || ($self->{lane} == 0 && $self->{x} > $light_position) || ($self->{lane} == 1 && $self->{y} > $light_position)) {
        if ($self->{lane} == 0) {  # Moviéndose hacia el Este
            $self->{x} += $current_speed;
            print "Moviéndose hacia el Este, nueva posición x: $self->{x}\n";
        } elsif ($self->{lane} == 1) {  # Moviéndose hacia el Norte
            $self->{y} += $current_speed;
            print "Moviéndose hacia el Norte, nueva posición y: $self->{y}\n";
        }
    } elsif ($traffic_light_state eq TrafficLight::RED && !$has_reached_light) {
        # No hacer nada, el coche se detiene
        print "Semáforo rojo y el coche no ha alcanzado el semáforo, el coche se detiene\n";
    }
}

sub draw {
    my ($self, $canvas) = @_;
    $canvas->createOval(
        $self->{x} - 5, $self->{y} - 5,
        $self->{x} + 5, $self->{y} + 5,
        -fill => 'blue'
    );
}

# Paquete principal
package main;

# Inicializar la ventana principal
my $mw = MainWindow->new;
$mw->title("Sincronización de Semáforos");
my $canvas = $mw->Canvas(-width => 400, -height => 400, -background => 'white')->pack;

# Variable para controlar el estado de la simulación
my $simulation_running = 1;

# Crear semáforos con tiempos sincronizados
# Semáforo A comienza en verde, Semáforo B comienza en rojo
my @traffic_lights = (
    TrafficLight->new(10, 1, 7, TrafficLight::GREEN, 0),  # Semáforo A (Verde: 10s, Amarillo: 1s, Rojo: 7s, comienza en verde)
    TrafficLight->new(10, 1, 7, TrafficLight::RED, 9),   # Semáforo B (Verde: 10s, Amarillo: 1s, Rojo: 7s, comienza en rojo, desfase de 9s)
);

# Crear coches
my @cars = (
    Car->new(0),   # Coche en el carril 0 (Este)
    Car->new(1),   # Coche en el carril 1 (Norte)
    Car->new(0),
    Car->new(1),
);

# Dibujar la intersección (solo las líneas)
sub draw_intersection {
    # Línea vertical (Norte-Sur)
    $canvas->createLine(200, 0, 200, 400, -width => 2, -fill => 'black');
    # Línea horizontal (Este-Oeste)
    $canvas->createLine(0, 200, 400, 200, -width => 2, -fill => 'black');
}

# Guardar el tiempo de inicio de la simulación
my $start_time = time;

# Actualizar la simulación
sub update_simulation {
    if ($simulation_running) {
        my $current_time = time;  # Obtener el tiempo actual
        my $elapsed_time = $current_time - $start_time;  # Calcular el tiempo transcurrido
        my $cycle_time = $elapsed_time % 18;  # Usar módulo para simular un ciclo repetitivo cada 18 segundos
        $canvas->delete('all');
        draw_intersection();

        # Actualizar los semáforos según el tiempo del ciclo
        foreach my $light (@traffic_lights) {
            $light->update($cycle_time);
            my $state = $light->get_state();
            my $color = $state eq TrafficLight::GREEN ? 'green' : $state eq TrafficLight::YELLOW ? 'yellow' : 'red';
            my ($x, $y) = $light == $traffic_lights[0] ? (150, 200) : (200, 150);  # Posiciones de los semáforos
            $canvas->createOval($x - 10, $y - 10, $x + 10, $y + 10, -fill => $color);
        }

        # Actualizar y dibujar coches
        foreach my $car (@cars) {
            my $light_state = $traffic_lights[$car->{lane}]->get_state();
            my $light_position = $car->{lane} == 0 ? 150 : 150;
            $car->update($light_state, $light_position);
            $car->draw($canvas);
        }
    }

    $mw->after(500, \&update_simulation);  # Actualizar cada 500 milisegundos
}

# Iniciar la simulación
update_simulation();
Tk::MainLoop;