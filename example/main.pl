use strict;
use warnings;
use Tk;

# Define agent class
package Agent;
sub new {
    my ($class, $startX, $startY) = @_;
    my $self = { x => $startX, y => $startY, state => 'Idle' };
    bless $self, $class;
    return $self;
}

sub move {
    my ($self, $width, $height) = @_;
    $self->{x} += (int(rand(3)) - 1);  # Move in x direction (-1, 0, 1)
    $self->{y} += (int(rand(3)) - 1);  # Move in y direction (-1, 0, 1)

    # Keep position within bounds
    $self->{x} = $self->{x} < 0 ? 0 : $self->{x} > $width - 1 ? $width - 1 : $self->{x};
    $self->{y} = $self->{y} < 0 ? 0 : $self->{y} > $height - 1 ? $height - 1 : $self->{y};
}

# Main program
my $mw = MainWindow->new;
$mw->title("Agent Movement Visualization");

# Set the size of the canvas (grid size)
my $canvas = $mw->Canvas(-width => 400, -height => 400, -bg => 'white');
$canvas->pack;

# Create agents
my $agent1 = Agent->new(0, 0);
my $agent2 = Agent->new(9, 9);

# Draw the grid (10x10 grid with each cell size 40x40)
for my $i (0..9) {
    for my $j (0..9) {
        $canvas->createRectangle($i * 40, $j * 40, ($i+1) * 40, ($j+1) * 40, -outline => 'black');
    }
}

# Draw the agents as circles
my $agent1_circle = $canvas->createOval($agent1->{x} * 40, $agent1->{y} * 40,
                                        ($agent1->{x} + 1) * 40, ($agent1->{y} + 1) * 40,
                                        -fill => 'red');
my $agent2_circle = $canvas->createOval($agent2->{x} * 40, $agent2->{y} * 40,
                                        ($agent2->{x} + 1) * 40, ($agent2->{y} + 1) * 40,
                                        -fill => 'blue');

# Update the positions of the agents every second
my $step = 0;
while ($step < 10) {
    # Move the agents
    $agent1->move(10, 10);
    $agent2->move(10, 10);

    # Update the canvas
    $canvas->coords($agent1_circle, $agent1->{x} * 40, $agent1->{y} * 40,
                                      ($agent1->{x} + 1) * 40, ($agent1->{y} + 1) * 40);
    $canvas->coords($agent2_circle, $agent2->{x} * 40, $agent2->{y} * 40,
                                      ($agent2->{x} + 1) * 40, ($agent2->{y} + 1) * 40);

    # Refresh the window
    $mw->update;
    sleep 1;  # Wait for 1 second before moving again

    $step++;
}

# MainLoop;

Tk::MainLoop;  # Fully qualified call to MainLoop